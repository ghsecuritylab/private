���ã���main.c�ļ�
//��ǰ�̼��汾
#define SYSTEM_FIIRMWARE_VERSION	0
//��ѯ�°汾�ӿ�
#define GET_FIRMWARE_VERSION_URL "http://113.107.235.123:8088/cgi-bin/autoupgrade.cgi"
//��ѯ��� Ĭ��30����
#define TIME_SLEEP_INTERVAL		60*30
//���ر���·��
char g_Default_path[256] = "/media/sda1";


����:
1. ��diyite_updater�ļ��п�����/home/5350/RT288x_SDK/source/userĿ¼
2. �޸�/home/5350/RT288x_SDK/source/user/Makefile�ļ�
���һ������"dir_y   += diyite_updater"
3. �޸�/home/5350/RT288x_SDK/source/vendors/Ralink/RT5350/rcS�ļ���
���diyite_updater&��������
4. ���ص�/home/5350/RT288x_SDK/sourceִ��make

