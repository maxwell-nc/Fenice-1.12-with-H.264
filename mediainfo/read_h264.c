#include <stdio.h>
#include <unistd.h>
#include <string.h>
 
#include <fenice/types.h>
#include <fenice/utils.h>
#include <fenice/itu_h264.h>
#include <fenice/log.h>
#include <fenice/en_xmalloc.h>
#include <include/fenice/itu_h264.h>
#include <include/fenice/mediainfo.h>

/**
 * 寻找3位的NAL起始码0x00 00 01
 */
static int is_start_code_3bit(char *data)
{
    if(data[0]!=0 || data[1]!=0 || data[2] !=1)
        return FALSE;
    else
        return TRUE;
}

/**
 * 寻找4位的NAL起始码0x00 00 00 01
 */
static int is_start_code_4bit(char *data)
{
    if(data[0]!=0 || data[1]!=0 || data[2] !=0 || data[3] !=1)
        return FALSE;
    else
        return TRUE;
}

/**
 * 获得一帧的NALU Payload部分
 * 一个原始的 H.264 NALU 单元常由 [Start Code] [NALU Header] [NALU Payload] 三部分组成
 */
static int get_one_NALU_payload(FILE *f ,NALU_t  *nalu)
{
    unsigned int pos = 0, start_code_len = 0;
    int info3 = 0, info4 = 0, find_another_start = 0, rewind = 0;
    char buff[MAX_FRAME_ZISE];

    if(3 != fread(buff, 1, 3, f))//从码流中读3个字节
        return ERR_EOF;
    if(is_start_code_3bit(buff)) {//判断是否为0x000001
        start_code_len = 3;
        pos = 3;
    } else {//如果不是，再读一个字节
        if(1 != fread(buff+3, 1, 1,f)){//读一个字节
            return ERR_EOF;
        }
        if(is_start_code_4bit(buff)){//判断是否为0x00000001
            start_code_len = 4;
            pos = 4;
        } else{
            return ERR_NOT_FOUND;//如果不是，返回找不到开始码
        }
    }

    while(!find_another_start) {
        
		if (feof(f)) {//判断是否到了文件尾
		
            nalu->forbidden_bit = buff[start_code_len] & 0x80;     // 1 bit 禁止位：网络发现NAL单元有比特错误时可设置该比特为1，以便接收方丢掉该单元
			
            nalu->nal_reference_idc = buff[start_code_len] & 0x60; // 2 bit	重要性指示：标志该NAL单元用于重建时的重要性，值越大，越重要。
			
            nalu->nal_unit_type = buff[start_code_len] & 0x1f;     // 5 bit	NALU类型：由此确定是否需要分包发送
			
            nalu->len = pos - start_code_len - 1;                  //去掉start code和NALU header
            
			memcpy(nalu->buf, &buff[start_code_len + 1], nalu->len);//拷贝一个完整NALU，不拷贝起始前缀0x000001或0x00000001
			
            if (0 != fseek (f, 0, SEEK_SET))//把文件指针指向开头
            {
                return ERR_EOF;
            }
			
        }
		
        buff[pos++] = (char)fgetc(f);//读一个字节到BUF中
        info4 = is_start_code_4bit(&buff[pos - 4]);//判断是否为0x00000001
        if (info4 != TRUE) {
            info3 = is_start_code_3bit(&buff[pos - 3]);//判断是否为0x000001
        }
        find_another_start = info3 || info4;
    }

	//找到另一个起始码，要往回偏移3或4bit
    rewind = info3 ? -3 : -4;


    if (0 != fseek (f, rewind, SEEK_CUR))//把文件指针指向前一个NALU的末尾，在当前文件指针位置上偏移 rewind。
    {
        return ERR_EOF;
    }

    nalu->forbidden_bit = buff[start_code_len] & 0x80;     // 1 bit
    nalu->nal_reference_idc = buff[start_code_len] & 0x60; // 2 bit
    nalu->nal_unit_type = buff[start_code_len] & 0x1f;     // 5 bit
    nalu->len = pos - start_code_len + rewind - 1;         //去掉start code NALU header和第二次的start code
    memcpy(nalu->buf, &buff[start_code_len + 1], nalu->len);//拷贝一个完整NALU，不拷贝起始前缀0x000001或0x00000001
	
    return ERR_NOERROR;
}

/**
 * 初始化内存空间
 */
static_h264* init_static_h264(media_entry * me)
{
    static_h264 *s;
    s = (static_h264*)malloc(sizeof(static_h264));
    s->f = fdopen(me->fd, "rb");
    s->n = malloc(sizeof(NALU_t));
    s->n->max_size = MAX_FRAME_ZISE;
    s->n->buf = malloc(MAX_FRAME_ZISE*sizeof(char));
    return s;
}

/**
 * 读取H264数据
 */
int read_h264(
        media_entry * me,
        uint8 * data,
        uint32 * data_size,
        double *mtime,
        int *recallme,
        uint8 * marker)
{
    int ret;

    static_h264 *s = NULL;

    FU_indicator    *fu_ind;
    FU_header       *fu_header;

    *marker = 1;

    if (!(me->flags & ME_FD)) {
        if ((ret = mediaopen(me)) < 0)//打开媒体
            return ret;
        s = init_static_h264(me);//分配内存
        me->stat = (void *) s;
    }

    s = (static_h264 *) me->stat;


    if(H264_NOT_SHARDING == s->sharding_packet) {//判断是否分片发送
	
        if (ERR_NOERROR != get_one_NALU_payload(s->f, s->n)) {//读取NALU_payload
            return ERR_EOF;
        }

        if (s->n->len <= 0) {//获取文件的长度
            return ERR_EOF;
        }

		//当一个NALU小于1400字节的时候，采用一个单RTP包发送
        if (s->n->len <= UDP_MAX_SIZE) {//NALU Payload大小小于UDP发送大小
		
            s->sharding_packet = H264_NOT_SHARDING;
            *recallme = 0;//不需要回调
            *marker = 1;
            *data_size = s->n->len + 1;
			
			//读取FU_indicator
            fu_ind = (FU_indicator *)&data[0];
            fu_ind->f = s->n->forbidden_bit;
            fu_ind->nri = s->n->nal_reference_idc >> 5;//值越大，表示当前NALU越重要
            fu_ind->type = s->n->nal_unit_type;
            memcpy(&data[1], s->n->buf, s->n->len);
			//处理时间戳，需要注意的是h264的采样率为90000HZ，因此时间戳的单位为1(秒)/90000，因此如果当前视频帧率为25fps，那时间戳间隔或者说增量应该为3600，怎么算出来的呢，每帧是1/25秒，那么这1/25秒有多少个时间戳单元呢，除以1/90000即可。而如果帧率为30fps，则增量为3000，以此类推。
            me->h264_time += (double)1000/me->description.frame_rate;
            *mtime = me->h264_time;

        } else {  //第一个包
		
            //处理时间戳
            me->h264_time +=  (double)1000/me->description.frame_rate;
            *mtime = me->h264_time;
            
			*recallme = 1;//需要回调
			
            s->sharding_packet = H264_SHARDING_PACKET;
            s->packet_num = s->n->len / UDP_MAX_SIZE;//包的数量
            s->last_packet_byte = s->n->len % UDP_MAX_SIZE;//最后一个包的大小
            s->current_packet = 0;//当前包
			
            *marker = 0;
            
			fu_ind = (FU_indicator *)&data[0];
            fu_ind->f = s->n->forbidden_bit;
            fu_ind->nri = s->n->nal_reference_idc >> 5;
            fu_ind->type = TYPE_FU_A;

			//设置FU_Header
            fu_header = (FU_header *)&data[1];
            fu_header->type = s->n->nal_unit_type;//与NALU的header中的Type类型一致
            fu_header->e = 0;//当设置成1,结束位指示分片NAL单元的结束。即荷载的最后字节是分片NAL单元的最后一个字节。当跟随的FU荷载不是分片NAL单元的最后分片,结束位设置为0。
            fu_header->r = 0;//保留位必须设置为0，接收者必须忽略该位。
            fu_header->s = 1;//当设置成1,开始位指示分片NAL单元的开始。当跟随的FU荷载不是分片NAL单元荷载的开始，开始位设为0

            memcpy(&data[2], s->n->buf, UDP_MAX_SIZE);
            *data_size = UDP_MAX_SIZE + 2;

            s->current_packet++;//当前包数加1

        }
    } else {  //分片发送，使用FU-A类型
	
        fu_ind = (FU_indicator *)&data[0];
        fu_ind->f = s->n->forbidden_bit;
        fu_ind->nri = s->n->nal_reference_idc >> 5;
        fu_ind->type = TYPE_FU_A;
		
        if(s->current_packet < s->packet_num){   // 非第一个包或最后一个包
            
			/*处理时间戳*/
            *mtime = me->h264_time;
            *marker = 0;
            *recallme = 1;
			
            fu_header = (FU_header *)&data[1];
            fu_header->type = s->n->nal_unit_type;
            fu_header->e = 0;//非结束
            fu_header->r = 0;//保留位必须设置为0，忽略
            fu_header->s = 0;//非开始

            memcpy(&data[2], s->n->buf + s->current_packet*1400, UDP_MAX_SIZE);
            *data_size = UDP_MAX_SIZE + 2;

            s->current_packet++;
			
        } else if(s->current_packet == s->packet_num){  //判断是否为最后一个

            *mtime = me->h264_time;

            s->sharding_packet = H264_NOT_SHARDING;//不再分片
            *marker = 1;
            *recallme = 1;
            fu_header = (FU_header *)&data[1];
            fu_header->type = s->n->nal_unit_type;
            fu_header->e = 1;//结束包
            fu_header->r = 0;//保留位必须设置为0，忽略
            fu_header->s = 0;//非开始

            memcpy(&data[2], s->n->buf + s->current_packet*1400, (size_t)s->last_packet_byte);
            *data_size = (uint32)s->last_packet_byte + 2;

        } else {
            return ERR_EOF;
        }
    }
    return ERR_NOERROR;
}

