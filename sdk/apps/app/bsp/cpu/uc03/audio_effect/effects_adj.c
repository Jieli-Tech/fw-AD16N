#include "typedef.h"
#include "irq.h"
#include "common.h"
#include "uart.h"
#include "vm.h"
#include "crc16.h"
#include "device.h"
#include "vfs.h"
#include "my_malloc.h"
#include "app_config.h"
#include "cfg_tools.h"
#include "effects_adj.h"
#include "audio_eq.h"
#include "usb/usr/usb_audio_interface.h"

#include "sys_memory.h"

#define LOG_TAG_CONST       NORM
/* #define LOG_TAG_CONST       OFF */
#define LOG_TAG             "[eff]"
#include "log.h"


const u8 eff_sdk_name[16] 	= "AD16N";
const u8 eff_eq_ver[4] 		= {1, 0, 0, 0};
static u32 effect_eq_len;//eq文件长度
static u8 is_eq_data_in_vm;//eq是否记录在VM
int read_eq_file_data(u8 *buffer, u32 file_len);
int read_eq_vm_data(u8 *buffer, u32 len);
extern u32 eq_begain[];

static u32 file_name_location;			//用来快速定位到bin文件里存储名字的位置
static u8 file_eq_group_num;			//用来保存bin文件里eq参数组组数

#define INDEX_HEAD  0xFFFD
#define EXT_HEAD    0xFFF7
#define NAME_HEAD    0xFFF6

/*----------------------------------------------------------------------*/
/*函数作用：音效模块的初始化函数，主要负责确认文件相关信息和记录关键信息*/
/*mode_id : 音效模块id,暂时固定使用0x4									*/
/*group_id : 音效模块id,暂时固定使用0x2001								*/
/*tar_snap ：目标样例序号（0：第一个样例信息；1：第二个样例信息；....） */
/*data_buf : 目标样例解析后填写的buf									*/
/*file_length ： 目标样例buf的长度											*/
/*----------------------------------------------------------------------*/
int eff_file_init(u32 mode_id, u16 group_id, void *data_buf, u32 buf_len, u32 file_length)
{
    u32 read_len = 0;
    int ret = 0;
    u8 *crc_buf = NULL;

    int err = 0;
    void *pfs = 0;
    void *pfile = 0;
    struct vfs_attr attr;
    err = vfs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        log_error("%s,mount err 0x%x  ", __FUNCTION__, err);
        return err;
    }
    err = vfs_openbypath(pfs, &pfile, CFG_EQ_FILE);
    if (err) {
        log_error("%s,openbypath err 0x%x  ", __FUNCTION__, err);
        return err;
    }
    vfs_get_attrs(pfile, &attr);				//获取文件相关信息

    u8 ver[4] = {0};
    vfs_seek(pfile, 1, SEEK_SET);
    vfs_read(pfile, ver, 4);
    if (memcmp(ver, eff_eq_ver, sizeof(eff_eq_ver))) {
        log_error("eff ver err \n");
        log_error_hexdump((unsigned char *)ver, 4);
        ret = -3;
        goto err_exit;
    }
    /* log_info("ver : \n "); */
    /* log_error_hexdump((unsigned char *)ver, 4); */

    u16 index_head = 0;//索引头
    vfs_read(pfile, &index_head, 2);
    if (index_head != INDEX_HEAD) {
        ret = -5;
        goto err_exit;
    }

    u16 index_len = 0;//索引区域长度
    vfs_read(pfile, &index_len, 2);
    /* log_info("index_len 0x%x   ",index_len); */

    u8 mode_num = 0;//模式个数
    vfs_read(pfile, &mode_num, 1);
    /* log_info("mode_num 0x%x   ",mode_num); */

    u16 off0 = 0;	//组索引区域的偏移
    u16 off1 = 0;	//模式索引区域的偏移
    for (int mode_cnt = 0; mode_cnt < mode_num; mode_cnt++) {
        u16 mode_seq  = 0;//模式标识
        vfs_read(pfile, &mode_seq, 2);
        /* log_info("mode_seq 0x%x   ",mode_seq); */

        vfs_read(pfile, &off0, 2);
        /* log_info("off0 0x%x   ",off0); */

        vfs_read(pfile, &off1, 2);
        /* log_info("off1 0x%x   ",off1); */

        if (mode_seq != mode_id) { //查询到相应的模式
            continue;
        } else {
            break;
        }
    }

    vfs_seek(pfile, off0, SEEK_SET);
    vfs_seek(pfile, 17, SEEK_CUR);

    /* 特殊操作，直接定位到EQ参数组总数位置 */
    vfs_read(pfile, &file_eq_group_num, 1);
    /* log_info("file_eq_group_num 0x%x  ",file_eq_group_num);	//获取bin文件中EQ参数组总数 */

    vfs_seek(pfile, 2 * (file_eq_group_num - 1), SEEK_CUR);			//直接偏移到最后一组

    vfs_read(pfile, &file_name_location, 2);
    /* log_info("file_name_location 0x%x   ",file_name_location);	//获取bin文件中EQ参数组总数 */


    vfs_seek(pfile, off0, SEEK_SET);					//偏移到组索引区域

    u8 group_num = 0;
    vfs_read(pfile, &group_num, 1);
    /* log_info("group_num 0x%x   ",group_num); */

    u16 g_id = 0;//模块id
    u16 group_id_addr = 0;//模块id的内容地址
    int group_cnt = 0;
    for (group_cnt = 0; group_cnt < group_num; group_cnt++) {
        vfs_read(pfile, &g_id, 2);
        /* log_info("g_id 0x%x   ",g_id); */

        vfs_read(pfile, &group_id_addr, 2);
        /* log_info("group_id_addr 0x%x   ",group_id_addr); */

        if (g_id != group_id) {
            continue;
        } else {
            break;
        }
    }
    if (group_cnt == group_num) {
        ret = -14;
        log_error("seek group id addr err\n");
        goto err_exit;
    }

    vfs_seek(pfile, group_id_addr, SEEK_SET);			//偏移到模式id的具体内容

    u16 crc16 = 0;
    u16 verify_group_id = 0;
    u16 group_len;

    vfs_read(pfile, &crc16, 2);

    int cur_addr = read_len;
    vfs_read(pfile, &verify_group_id, 2);			//id组数

    vfs_read(pfile, &group_len, 2);

    if (!group_len) {
        ret = -18;
        goto err_exit;
    }

    if (verify_group_id != group_id) {
        ret = -19;
        log_error("verify_group_id %x != group_id %x\n", verify_group_id, group_id);
        log_error("verify groud id err\n");
        goto err_exit;
    }

    vfs_seek(pfile, group_id_addr, SEEK_SET);
    vfs_seek(pfile, 2, SEEK_CUR);

    u16 size = sizeof(crc16) + sizeof(group_len);
    crc_buf = my_malloc(group_len  + size, MM_CFG);
    /* log_info(" my malloc size : 0x%x  ", size); */
    u8 *group_buf = &crc_buf[size];
    vfs_read(pfile, crc_buf, group_len + size);

    u16 calc_crc = CRC16(crc_buf, group_len + size); //id + len +playload
    if (crc16 != calc_crc) {
        ret = -21;
        log_error("crc16 %x, calc_crc %x\n", crc16, calc_crc);
        goto err_exit;
    }
    if (!data_buf) {
        ret = -22;
        log_error("input data buf NULL\n");
        goto err_exit;
    }
    if (group_len <= buf_len) {
        memcpy(data_buf, group_buf, group_len);
    } else {
        ret = -23;
        log_error("input data buf len : %d < group_len:%d\n", buf_len, group_len);
        goto err_exit;
    }

err_exit:
    if (crc_buf) {
        my_free(crc_buf);
        crc_buf = NULL;
    }
    if (ret) {
        log_error("ret :%d, analyze eff file err, please check it\n", ret);
    }
    return ret;
}

/*----------------------------------------------------------------------*/
/*函数作用：音效模块的样例切换解析函数接口 								*/
/*mode_id : 音效模块id													*/
/*group_id : 音效模块id													*/
/*tar_snap ：目标样例序号（0：第一个样例信息；1：第二个样例信息；....） */
/*data_buf : 目标样例解析后填写的buf									*/
/*buf_len ： 目标样例buf的长度											*/
/*----------------------------------------------------------------------*/
int ext_eff_file_analyze(u32 mode_id, u16 group_id, u8 tar_snap, void *data_buf, u32 buf_len)
{
    int ret = 0;
    int err = 0;
    void *pfs = 0;
    void *pfile = 0;
    struct vfs_attr attr;
    err = vfs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        log_error("%s,mount err 0x%x  ", __FUNCTION__, err);
        return err;
    }
    err = vfs_openbypath(pfs, &pfile, CFG_EQ_FILE);
    if (err) {
        log_error("%s,openbypath err 0x%x  ", __FUNCTION__, err);
        return err;
    }
    vfs_get_attrs(pfile, &attr);				//获取文件相关信息

    u32 read_len = 0;
    u8 ver[4] = {0};
    vfs_seek(pfile, 1, SEEK_SET);

    vfs_read(pfile, ver, 4);
    if (memcmp(ver, eff_eq_ver, sizeof(eff_eq_ver))) {
        log_error("eff ver err \n");
        log_error_hexdump((unsigned char *)ver, 4);
        ret = -3;
        goto __ext_eff_file_exit;
    }


    u16 index_head = 0;
    vfs_seek(pfile, 5, SEEK_SET);
    vfs_read(pfile, &index_head, 2);
    /* log_info("index_head = 0x%x   ",index_head); */
    if (index_head != INDEX_HEAD) {
        ret = -5;
        goto __ext_eff_file_exit;
    }

    u16 index_len = 0;//索引区域长度
    vfs_read(pfile, &index_len, 2);
    /* log_info("index_len = 0x%x    ",index_len); */
    vfs_seek(pfile, index_len, SEEK_CUR);



    u16 ext_head = 0;
    vfs_read(pfile, &ext_head, 2);
    /* log_info("ext_head = 0x%x   ",ext_head); */
    if (ext_head != EXT_HEAD) {
        ret = -8;
        goto __ext_eff_file_exit;
    }

/////快照索引区长度
    u16 ext_head_len = 0;
    vfs_read(pfile, &ext_head_len, 2);
    /* log_info("ext_head_len = 0x%x  ",ext_head_len); */


/////
    u16 CNT = 0;
    vfs_read(pfile, &CNT, 2);
    /* log_info("CNT = 0x%x   ",CNT); */

/////
    u16 group = 0;
    u16 offset = 0;
    u16 mode_seq = 0;
    int group_cnt = 0;
    for (group_cnt  = 0; group < CNT ; group_cnt++) {
        mode_seq = 0;
        vfs_read(pfile, &mode_seq, 2);
        /* log_info("mode_seq = 0x%x   ",mode_seq); */

        vfs_read(pfile, &group, 2);
        /* log_info("group = 0x%x   ",group); */

        vfs_read(pfile, &offset, 2);
        /* log_info("offset = 0x%x   ",offset); */
        /* log_info(" mode_seq 0x%x ;mode_id 0x%x ;group_id 0x%x ;group 0x%x ",mode_seq,mode_id,group_id,group);	 */
        if ((mode_seq == mode_id) && (group_id == group)) {
            /* log_info(" group_id snap ok 0x%x ",group); */
            break;
        }
    }
    if (group_cnt == CNT) {
        ret = -24;
        goto __ext_eff_file_exit;
    }


    vfs_seek(pfile, offset, SEEK_SET);	//跳转到快照索引区域
    u8 snap_group_num = 0;
    vfs_read(pfile, &snap_group_num, 1);
    /* log_info("snap_group_num = 0x%x   ",snap_group_num); */

    if (tar_snap > snap_group_num) {	//如果输入的EQ组数超过最大组数，退出,(好像是应该大于等于，后续再看)
        ret = -15;
        goto __ext_eff_file_exit;
    }


    u8 snap_cnt = 0;
    u16 snap_group_offset = 0;
    for (snap_cnt = 0; snap_cnt < snap_group_num; snap_cnt++) {
        vfs_read(pfile, &snap_group_offset, 2);
        /* log_info("snap_group_offset = 0x%x   ",snap_group_offset); */
        if (snap_cnt == tar_snap) {
            break;
        }
    }

    if (snap_cnt == snap_group_num) {		//输入的eq组数超过bin文件里保存的eq参数组数
        ret = -17;
        goto __ext_eff_file_exit;
    }

    vfs_seek(pfile, snap_group_offset, SEEK_SET); //跳转到对应eq组数的快照偏移地址
    u16 snap_group_len = 0;
    vfs_read(pfile, &snap_group_len, 2);			//跳转过后读两个byte为这组eq参数的长度
    /* log_info("snap_group_len = 0x%x  buf_len 0x%x  ",snap_group_len,buf_len); */

    if (snap_group_len <= buf_len) {
        vfs_read(pfile, data_buf, snap_group_len);//如果长度符合要求，则把bin文件里的eq参数读取放入data_buf
    } else {
        ret = -19;
        /* log_info(" buf_len 0x%x ; snap_group_len 0x%x ",buf_len,snap_group_len); */
        goto __ext_eff_file_exit;
    }
__ext_eff_file_exit:
    vfs_file_close(&pfile);
    vfs_fs_close(&pfs);
    return ret;
}

int ext_eff_file_analyze_api(u8 eq_mode, void *data_buf, u32 buf_len)
{
    return ext_eff_file_analyze(music_mode_seq0, EFF_MUSIC_EQ, eq_mode, data_buf, buf_len);
}


/*----------------------------------------------------------------------*/
/*函数作用 ：获取eq在线调试工具生成的bin文件里面样例总数,即有多少组EQ参数*/
/*----------------------------------------------------------------------*/
u8 get_eq_mode_total_num()
{
    return file_eq_group_num;
}

/*----------------------------------------------------------------------*/
/*函数作用 ：获取对应数字的EQ组参数的名称 								*/
/*mode_id  : 音效模块id													*/
/*group_id : 音效模块id													*/
/*tar_snap ：目标样例序号（0：第一个样例信息；1：第二个样例信息；....） */
/*----------------------------------------------------------------------*/
static void *get_eq_mode_name(u32 mode_id, u16 group_id, u8 tar_snap)
{
    void *pfs = 0;
    void *pfile = 0;
    u32 err = 0;
    err = vfs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        goto __get_file_name_exit;
    }
    err = vfs_openbypath(pfs, &pfile, CFG_EQ_FILE);
    if (err) {
        goto __get_file_name_exit;
    }
    vfs_seek(pfile, file_name_location, SEEK_SET);		//直接偏移到最后一组EQ参数开头
    /* log_info("file_name_location 0x%x  ",file_name_location);  */

    /*
    G1_LEN:第一组EQ参数数据长度,占两个byte
    G2_LEN:第二组EQ参数数据长度,占两个byte
    Gn_LEN:第n组EQ参数数据长度,占两个byte
    file_name_location:最后一组EQ参数头
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    |G1_LEN	|	...data...  |G2_LEN	|	...data...  |Gn_LEN	|	...data...  |
    +---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
    */

    u16 snap_group_len = 0;
    vfs_read(pfile, &snap_group_len, 2);			//读取两个byte,得到对应eq参数组的大小
    /* log_info(" snap_group_len 0x%x  ",snap_group_len); */
    vfs_seek(pfile, snap_group_len, SEEK_CUR);

    /*
    0xFFF6 	: bin文件存储样例名字区域,即NAME_HEAD
    LEN		: 存储名字区域的大小
    NUM		: 多少个样例名字
    L1		: 第一个样例的名字的长度
    NAME1	: 第一个样例的名字
    Ln		: 第n个样例的名字的长度
    NAMEn	: 第n个样例的名字
    +---+---+---+---+---+---+--...--+---+--...--+---+---+---+
    |F6   FF|  LEN  |NUM|L1 | NAME1	|L2 | NAME2 |Ln | NAMEN |
    +---+---+---+---+---+---+--...--+---+--...--+---+---+---+
    */
    u16 name_head = 0;
    vfs_read(pfile, &name_head, 2);					//读取两个byte，观察是否去到了存储样例名字区域的标识符0xFFF6
    /* log_info(" name_head 0x%x  ",name_head); */

    if (name_head != NAME_HEAD) {
        log_error("read bin file EQ group name head wrong , now name_head 0x%x ", name_head);
        return  NULL;
    }

    vfs_seek(pfile, 2, SEEK_CUR);

    u8 eq_name_group_num = 0;
    vfs_read(pfile, &eq_name_group_num, 1);  		//得到样例名字总数,即NUM
    /* log_info(" eq_name_group_num 0x%x  ",eq_name_group_num); */

    u8 eq_name_group_len = 0;
    u8 eq_name[16] = {0};							//实际名字可以取到256个byte，但这里为了节省只取了16个byte
    for (u8 i = 0; i < eq_name_group_num; i++) {						//for循环不断去找到对应样例名字的位置
        vfs_read(pfile, &eq_name_group_len, 1);
        /* log_info(" eq_name_group_len 0x%x  ",eq_name_group_len); */
        if (i == tar_snap) {
            vfs_read(pfile, &eq_name, eq_name_group_len);
            /* log_info(" eq_name %s  ",eq_name); */
            break;
        }
        vfs_seek(pfile, eq_name_group_len, SEEK_CUR);

    }
    void *name = eq_name;

    vfs_file_close(&pfile);
    vfs_fs_close(&pfs);
    return name;

__get_file_name_exit:
    vfs_file_close(&pfile);
    vfs_fs_close(&pfs);
    return NULL;
}

void *get_eq_mode_name_api(u8 eq_mode)
{
    return get_eq_mode_name(music_mode_seq0, EFF_MUSIC_EQ, eq_mode);
}



void music_eq_printf(void *_parm)
{
    struct eq_seg_info *seg;
    struct music_eq_tool *parm =  _parm;
    log_debug("----------------------- music eq ----------------------\n");
    log_debug("global_gain:0x%x\n", *(int *)&parm->global_gain);
    log_debug("seg_num:%d\n", parm->seg_num);
    for (int i = 0; i < parm->seg_num; i++) {
        seg = &parm->seg[i];
        log_debug("idx:%d, iir:%d, frq:%d, gain:0x%x, q:0x%x \n", seg->index, seg->iir_type, seg->freq, *(int *)&seg->gain, *(int *)&seg->q);
    }
}

void effect_eq_parm_init(void)
{
    int ret;
    u32 read_len;
    void *tar_buf = &music_eq_tool_online;
    u16 tar_len = sizeof(struct music_eq_tool);

    effect_eq_len = get_eq_file_len();					//effect_eq_len获取文件长度
    is_eq_data_in_vm = 0;
    if (effect_eq_len) {
        //对eq文件数据进行解释,并设置到eq数组
        ret = eff_file_init(music_mode_seq0, EFF_MUSIC_EQ, tar_buf, tar_len, effect_eq_len);//解析文件数据
        if (ret == 0) {
            /* music_eq_printf(tar_buf); */
            log_info("effect_eq_parm_init ok");
        } else {
            log_info("effect_eq_parm_init err : 0x%x  ", ret);
        }
    } else {											//没有对应的dir_res文件
        log_error("no eq file! plase check!\n");
    }
}


int get_eq_file_len(void)
{
    u32 err = 0;
    u32 file_len = 0;
    void *pfs = 0;
    void *pfile = 0;
    struct vfs_attr attr;
    err = vfs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        goto __err_exit;
    }
    err = vfs_openbypath(pfs, &pfile, CFG_EQ_FILE);
    if (err) {
        goto __err_exit;
    }
    vfs_get_attrs(pfile, &attr);
    file_len = attr.fsize;
__err_exit:
    vfs_file_close(&pfile);
    vfs_fs_close(&pfs);
    return file_len;
}

int read_eq_file_data(u8 *buffer, u32 file_len)
{
    u32 err = 0;
    u32 read_len = 0;
    void *pfs = 0;
    void *pfile = 0;
    err = vfs_mount(&pfs, (void *)NULL, (void *)NULL);
    if (err) {
        goto __err_exit;
    }
    err = vfs_openbypath(pfs, &pfile, CFG_EQ_FILE);
    if (err) {
        goto __err_exit;
    }
    read_len = vfs_read(pfile, buffer, file_len);
__err_exit:
    vfs_file_close(&pfile);
    vfs_fs_close(&pfs);
    return read_len;
}

int read_eq_vm_data(u8 *buffer, u32 len)
{
    int err;
    err = sysmem_read_api(SYSMEM_INDEX_EQ, buffer, len);
    if (err <= 0) {
        log_info("vm err: %d\n", err);
        return 0;
    }
    return err;
}

int write_eq_vm_data(u8 *buffer, u32 len)
{
    int err;
    if (effect_eq_len == 0 || (len != effect_eq_len)) {
        log_error("effect_eq_len: %d, len: %d\n", effect_eq_len, len);
        return 0;
    }
    err = sysmem_write_api(SYSMEM_INDEX_EQ, buffer, len);
    if (err <= 0) {
        log_info("vm err: %d\n", err);
        return 0;
    }
    is_eq_data_in_vm = 1;
    return err;
}

//读取eq数据,自动选择从VM或者文件读取
int effect_get_cfg_file_data(u32 offset, u8 *data, u32 len)
{
    u32 read_len;
    u8 *file_buffer = NULL;
    if ((effect_eq_len == 0) || ((offset + len) > effect_eq_len)) {
        return -EINVAL;
    }
    file_buffer = my_malloc(effect_eq_len, MM_CFG);
    if (!file_buffer) {
        return -EINVAL;
    }
    if (is_eq_data_in_vm) {
        read_len = read_eq_vm_data(file_buffer, effect_eq_len);
    } else {
        read_len = read_eq_file_data(file_buffer, effect_eq_len);
    }
    if (read_len != effect_eq_len) {
        my_free(file_buffer);
        return -EINVAL;
    }
    memcpy(data, file_buffer + offset, len);
    my_free(file_buffer);
    return len;
}


#if TCFG_CFG_TOOL_ENABLE

void eff_send_packet(void *priv, u32 id, u8 *packet, int size, u8 sq)
{
    all_assemble_package_send_to_pc(id, sq, packet, size);
}

int eff_tool_get_version(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    struct _ver_info {
        char sdkname[16];
        u8 eqver[4];
    };
    struct _ver_info _ver_info = {0};
    memcpy(_ver_info.sdkname, eff_sdk_name, sizeof(eff_sdk_name));
    memcpy(_ver_info.eqver, eff_eq_ver, sizeof(eff_eq_ver));
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)&_ver_info, sizeof(struct _ver_info), sq);
    return 1;
}

int eff_tool_get_cfg_file_size(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    struct file_s {
        int id;
        int fileid;
    };
    struct file_s fs;
    memcpy(&fs, packet, sizeof(struct file_s));
    u32 file_size  = 0;
    if (fs.fileid == EFF_CFG_FILE_ID) {
        file_size = effect_eq_len;
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)&file_size, sizeof(u32), sq);
    }
    return 1;
}

int eff_tool_get_cfg_file_data(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    struct file_s {
        int id;
        int fileid;
        int offset;
        int size;
    };
    struct file_s fs;
    u8 *data;
    u32 read_len;
    memcpy(&fs, packet, sizeof(struct file_s));
    if (fs.fileid == (int)EFF_CFG_FILE_ID) {
        if (effect_eq_len == 0) {
            return -EINVAL;
        }
        data = my_malloc(fs.size, MM_CFG);
        if (!data) {
            return -EINVAL;
        }

        read_len = effect_get_cfg_file_data(fs.offset, data, fs.size);
        if (read_len != fs.size) {
            my_free(data);
            return -EINVAL;
        }
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)data, fs.size, sq);
        my_free(data);
    }
    return 1;
}

//在线检测应答
int eff_tool_set_inquire(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    //eff_send_packet(NULL, id, (u8 *)"OK", 2, sq);//OK表示需要重传，NO表示不需要重传,ER还是表示未知命令
    eff_send_packet(NULL, id, (u8 *)"NO", 2, sq);//OK表示需要重传，NO表示不需要重传,ER还是表示未知命令
    return 1;
}

int eff_tool_set_channge_mode(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
    return 1;
}

int music_eq_parm_analyze(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    float global_gain = 0;
    u8 mode_id = packet->data[0];
    void *tar_seg = NULL;

    struct eq_seg_info *seg = (struct eq_seg_info *)&packet->data[1];

    log_debug("mode_id %d\n", mode_id);
    if (seg->index == (u16) - 1) {
        memcpy(&global_gain, &seg->iir_type, sizeof(float));
        log_info("global_gain 0x%x\n", *(int *)&global_gain);
        music_eq_tool_online.global_gain = global_gain;
    } else {
        tar_seg = seg;
        log_debug("index: %d\n", seg->index);
        log_debug("freq : %d\n", seg->freq);
        log_debug("gain : %d\n", (int)(seg->gain * 1000));
        log_debug("q    : %d\n", (int)(seg->q * 1000));
        memcpy(&music_eq_tool_online.seg[seg->index], tar_seg, sizeof(struct eq_seg_info));
    }
    eq_online_sw(get_usb_eq_handle());
    return 0;
}

int eff_tool_resync_parm_begin(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
    return 1;
}

int eff_tool_resync_parm_end(EFF_ONLINE_PACKET *packet, u8 id, u8 sq)
{
    eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
    return 1;
}

/**
 *注册个别查询命令解析函数,返回值1成功，返回小于0失败
 * */
REGISTER_CMD_TARGET(version) = {
    .cmd = EFF_CMD_GETVER,
    .cmd_deal = eff_tool_get_version,
};
REGISTER_CMD_TARGET(file_s) = {
    .cmd = EFF_CMD_FILE_SIZE,
    .cmd_deal = eff_tool_get_cfg_file_size,
};
REGISTER_CMD_TARGET(file_p) = {
    .cmd = EFF_CMD_FILE,
    .cmd_deal = eff_tool_get_cfg_file_data,
};
REGISTER_CMD_TARGET(change_mode) = {
    .cmd = EFF_CMD_CHANGE_MODE,
    .cmd_deal = eff_tool_set_channge_mode,
};
REGISTER_CMD_TARGET(inquire) = {
    .cmd = EFF_CMD_INQUIRE,
    .cmd_deal = eff_tool_set_inquire,
};
REGISTER_CMD_TARGET(music_eq) = {//music eq
    .cmd = EFF_MUSIC_EQ,
    .cmd_deal = music_eq_parm_analyze,
};
REGISTER_CMD_TARGET(resync_begin) = {
    .cmd = EFF_CMD_RESYNC_PARM_BEGIN,
    .cmd_deal = eff_tool_resync_parm_begin,
};
REGISTER_CMD_TARGET(resync_end) = {
    .cmd = EFF_CMD_RESYNC_PARM_END,
    .cmd_deal = eff_tool_resync_parm_end,
};

static s32 eff_online_update(void *_packet)
{
    u8 *ptr = _packet;
    u8 id = ptr[0];
    u8 sq = ptr[1];
    int res = -1;
    EFF_ONLINE_PACKET *packet = (EFF_ONLINE_PACKET *)&ptr[2];
    struct cmd_interface *p = NULL;

    log_info("%s, packet_cmd: 0x%x\n", __FUNCTION__, packet->cmd);

    list_for_each_cmd_interface(p) {
        if (p->cmd == packet->cmd) {
            if (p->cmd_deal) {
                res = p->cmd_deal(packet, REPLY_TO_TOOL, sq);
            }
        }
    }
    return res;
}

static void effect_tool_callback(u8 *_packet, u32 size)
{
    int res = 0;
    u8 *ptr = _packet;
    u8 id = ptr[0];
    u8 sq = ptr[1];
    u8 *packet = (u8 *)&ptr[2];
    ASSERT(((int)packet & 0x3) == 0, "buf %x size %d\n", (unsigned int)packet, size - 2);
    res = eff_online_update(ptr);
    if (!res) {
        log_debug("Ack");
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"OK", 2, sq);
    } else if (res != 1) {
        log_debug("Nack");
        eff_send_packet(NULL, REPLY_TO_TOOL, (u8 *)"ER", 2, sq);
    }
}

REGISTER_DETECT_TARGET(eff_adj_target) = {
    .id         = 0x11,//EFF_CONFIG_ID,
    .tool_message_deal   = effect_tool_callback,
};

#endif


#if 0
void effect_eq_parm_init(void)
{
    int ret;
    u32 read_len;
    u8 *eq_data = NULL;
    void *tar_buf = &music_eq_tool_online;
    u16 tar_len = sizeof(struct music_eq_tool);

#if USE_FLASH_DEBUG
    effect_eq_len = get_eq_file_len();
    is_eq_data_in_vm = 0;
    if (effect_eq_len) {
        eq_data = my_malloc(effect_eq_len, MM_CFG);
        if (eq_data) {
            //读取vm的数据
            read_len = read_eq_vm_data(eq_data, effect_eq_len);
            if (read_len != effect_eq_len) {
                //再读取文件数据
                read_eq_file_data(eq_data, effect_eq_len);
            } else {
                is_eq_data_in_vm = 1;
            }
        }
        //对eq文件数据进行解释,并设置到eq数组
        ret = eff_file_analyze(music_mode_seq0, EFF_MUSIC_EQ, tar_buf, tar_len, eq_data, effect_eq_len);
        if (ret == 0) {
            music_eq_printf(tar_buf);
        }
    } else {
        log_error("no eq file! plase check!\n");
    }
    my_free(eq_data);
#else
    //otp时获取eq文件
    //对eq文件数据进行解释,并设置到eq数组
    ret = eff_file_analyze(music_mode_seq0, EFF_MUSIC_EQ, tar_buf, tar_len, (u8 *)eq_begain, 512);
    if (ret == 0) {
        music_eq_printf(tar_buf);
    }
#endif
}
#endif

#if 0
int eff_file_analyze(u32 mode_id, u16 group_id, void *data_buf, u32 buf_len, u8 *file_buffer, u32 file_length)
{
    u32 read_len = 0;
    int ret = 0;
    u8 *crc_buf = NULL;
    log_debug("mode_id 0x%x, group_id 0x%x\n", mode_id, group_id);

    read_len += 1;
    u8 ver[4] = {0};
    memcpy(ver, file_buffer + read_len, 4);
    read_len += 4;
    if (memcmp(ver, eff_eq_ver, sizeof(eff_eq_ver))) {
        log_error("eff ver err \n");
        log_error_hexdump((unsigned char *)ver, 4);
        ret = -3;
        goto err_exit;
    }

    u16 index_head = 0;//索引头
    memcpy(&index_head, file_buffer + read_len, 2);
    read_len += 2;
    if (index_head != INDEX_HEAD) {
        ret = -5;
        goto err_exit;
    }

    u16 index_len = 0;//索引区域长度
    memcpy(&index_len, file_buffer + read_len, 2);
    read_len += 2;
    u8 mode_num = 0;//模式个数
    memcpy(&mode_num, file_buffer + read_len, 1);
    read_len += 1;

    u16 off0 = 0;   //组索引区域的偏移
    u16 off1 = 0;   //模式索引区域的偏移
    for (int mode_cnt = 0; mode_cnt < mode_num; mode_cnt++) {
        u16 mode_seq  = 0;//模式标识
        memcpy(&mode_seq, file_buffer + read_len, 2);
        read_len += 2;

        memcpy(&off0, file_buffer + read_len, 2);
        read_len += 2;

        memcpy(&off1, file_buffer + read_len, 2);
        read_len += 2;

        if (mode_seq != mode_id) { //查询到相应的模式
            continue;
        } else {
            break;
        }
    }

    read_len = off0;//偏移到组索引区域

    u8 group_num = 0;
    memcpy(&group_num, file_buffer + read_len, 1);
    read_len += 1;

    u16 g_id = 0;//模块id
    u16 group_id_addr = 0;//模块id的内容地址
    int group_cnt = 0;
    for (group_cnt = 0; group_cnt < group_num; group_cnt++) {
        memcpy(&g_id, file_buffer + read_len, 2);
        read_len += 2;

        memcpy(&group_id_addr, file_buffer + read_len, 2);
        read_len += 2;

        if (g_id != group_id) {
            continue;
        } else {
            break;
        }
    }
    if (group_cnt == group_num) {
        ret = -14;
        log_error("seek group id addr err\n");
        goto err_exit;
    }
    read_len = group_id_addr;//偏移到模式id的具体内容

    u16 crc16 = 0;
    u16 verify_group_id = 0;
    u16 group_len;

    memcpy(&crc16, file_buffer + read_len, 2);
    read_len += 2;

    int cur_addr = read_len;
    memcpy(&verify_group_id, file_buffer + read_len, 2);
    read_len += 2;

    memcpy(&group_len, file_buffer + read_len, 2);
    read_len += 2;

    if (!group_len) {
        ret = -18;
        goto err_exit;
    }

    if (verify_group_id != group_id) {
        ret = -19;
        log_error("verify_group_id %x != group_id %x\n", verify_group_id, group_id);
        log_error("verify groud id err\n");
        goto err_exit;
    }

    read_len = cur_addr;
    u16 size = sizeof(crc16) + sizeof(group_len);
    crc_buf = my_malloc(group_len  + size, MM_CFG);
    u8 *group_buf = &crc_buf[size];
    memcpy(crc_buf, file_buffer + read_len, group_len + size);

    u16 calc_crc = CRC16(crc_buf, group_len + size); //id + len +playload
    if (crc16 != calc_crc) {
        ret = -21;
        log_error("crc16 %x, calc_crc %x\n", crc16, calc_crc);
        goto err_exit;
    }
    if (!data_buf) {
        ret = -22;
        log_error("input data buf NULL\n");
        goto err_exit;
    }
    if (group_len <= buf_len) {
        memcpy(data_buf, group_buf, group_len);
    } else {
        ret = -23;
        log_error("input data buf len : %d < group_len:%d\n", buf_len, group_len);
        goto err_exit;
    }

err_exit:
    if (crc_buf) {
        my_free(crc_buf);
        crc_buf = NULL;
    }
    if (ret) {
        log_error("ret :%d, analyze eff file err, please check it\n", ret);
    }
    return ret;
}
#endif
