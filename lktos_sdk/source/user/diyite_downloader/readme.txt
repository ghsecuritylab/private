���ã���main.c�ļ�
//�������ص�ʱ���
#define TIME_DOWNLOAD_BEGIN   2
#define TIME_DOWNLOAD_END	4
//��ȡapk�б�ӿ�
#define APK_LIST_BY_MAC_URL "http://113.107.235.123/firste/boxDownload?boxMac="
//����SD��·��
#define SD_PATH "/media/sda1"


����:
1. ��diyite_downloader�ļ��п�����/home/5350/RT288x_SDK/source/userĿ¼
2. �޸�/home/5350/RT288x_SDK/source/user/Makefile�ļ�
���һ������"dir_y   += diyite_downloader"
3. �޸�/home/5350/RT288x_SDK/source/vendors/Ralink/RT5350/rcS�ļ���
���diyite_downloader&��������
4. ���ص�/home/5350/RT288x_SDK/sourceִ��make

ע�⣺
��Ҫʹ��wget������make menuconfig�д�wget
