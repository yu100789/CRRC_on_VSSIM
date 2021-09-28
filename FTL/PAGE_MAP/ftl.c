// File: ftl.c
// Date: 18-Sep-2017
// Author: Jinsoo Yoo (jedisty@hanyang.ac.kr)
// Copyright(c)2017
// Hanyang University, Seoul, Korea
// Embedded Software Systems Laboratory. All right reserved

#include "common.h"

int g_init = 0;
int g_term = 0;
unsigned long int total_pages = 0;
unsigned long int total_page_read = 0;
uint64_t total_lpn_w = 0, total_lpn_r = 0;
uint32_t total_5LC = 0;
uint32_t total_6LC = 0;
uint32_t total_7LC = 0;
uint32_t total_8LC = 0;
uint64_t total_updates = 0;
double overall_AverageSymbols = 0;
double compressratio = 0;
pthread_mutex_t term_lock = PTHREAD_MUTEX_INITIALIZER;

static unsigned char *w_bloomfilter = NULL, *r_bloomfilter = NULL;

FILE *fp_ch_util;
FILE *fp_w_event;
FILE *fp_wb_lat;
FILE *fp_rw_lat;
page_buffer *p_buffer;
HOT_LRU *hot_r_lru, *hot_w_lru;
/* return value of each init function 
 * 	-1: return FAIL
 *	 0: First boot
 *	 1: Initialize from metadata files
 */
void FTL_INIT(void)
{
	int ret;
	if (g_init == 0)
	{
		printf("[%s] start\n", __FUNCTION__);

		INIT_SSD_CONFIG();

		ret = INIT_PERF_CHECKER();
		if (ret == -1)
			goto fail;

		INIT_IO_BUFFER();

		/*
		Modified 2021/05/15
		*/
		INIT_PAGE_BUFFER();
		INIT_HOT_LRU();
		w_bloomfilter = (unsigned char *)calloc(1, 40000000 * sizeof(unsigned char));
		r_bloomfilter = (unsigned char *)calloc(1, 40000000 * sizeof(unsigned char));
		ret = INIT_FLASH_INFO(ret);
		if (ret == -1)
			goto fail;

		INIT_VSSIM_CORE(); /* Init Flash -> Init Core */

		ret = INIT_MAPPING_TABLE(ret); /* Init Core -> Init Mapping */
		if (ret == -1)
			goto fail;

#ifdef MONITOR_ON
		INIT_LOG_MANAGER();
#endif
		INIT_FLASH();

		g_init = 1;
		printf("[%s] complete\n", __FUNCTION__);

#ifdef GET_CH_UTIL_INFO
		fp_ch_util = fopen("./ch_util_info.txt", "a");
		if (fp_ch_util == NULL)
		{
			printf("ERROR[%s] open ch_util_info file fail\n", __FUNCTION__);
		}
#endif
#ifdef GET_W_EVENT_INFO
		fp_w_event = fopen("./w_event_info.txt", "a");
		if (fp_w_event == NULL)
		{
			printf("ERROR[%s] open w_event_info file fail\n", __FUNCTION__);
		}
#endif
#ifdef GET_WB_LAT_INFO
		fp_wb_lat = fopen("./wb_lat_info.txt", "a");
		if (fp_wb_lat == NULL)
		{
			printf("ERROR[%s] open wb_lat_info file fail\n", __FUNCTION__);
		}
#endif
#ifdef GET_RW_LAT_INFO
		fp_rw_lat = fopen("./rw_lat_info.txt", "a");
		if (fp_rw_lat == NULL)
		{
			printf("ERROR[%s] open rw_lat_info file fail\n", __FUNCTION__);
		}
#endif

		return;

	fail:
		printf("[%s] init fail\n", __FUNCTION__);
		TERM_VSSIM_CORE();
		return;
	}
}
static void dump_total_writes(void)
{
	char str[50];
	sprintf(str, "dump_lba/total_writes_info.txt");
	int fd = open(str, O_WRONLY | O_CREAT, 0777);
	if (fd < 0)
	{
		perror("open tmp failed");
		return;
	}
	memset(str, 0, 50);
	sprintf(str, "Total_Write_Pages: %ld\n", GET_TOTAL_WRITE_PAGES());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "total_Read_Pages: %ld\n", GET_TOTAL_READ_PAGES());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_WRITE_LPN: %ld\n", GET_TOTAL_WRITE_LPN());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_READ_LPN: %ld\n", GET_TOTAL_READ_LPN());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_5LC: %d\n", GET_TOTAL_5LC());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_6LC: %d\n", GET_TOTAL_6LC());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_7LC: %d\n", GET_TOTAL_7LC());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Total_8LC: %d\n", GET_TOTAL_8LC());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Average_Bits_Per_Symbols: %f\n", GET_ENTROPY());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	memset(str, 0, 50);
	sprintf(str, "Compress_Ratio: %f\n", GET_CR());
	if (!write(fd, str, 50))
		perror("write tmp failed");
	close(fd);
}
void FTL_TERM(void)
{
	pthread_mutex_lock(&term_lock);

	if (g_term == 0)
	{
		g_term = 1;

		printf("[%s] start\n", __FUNCTION__);
		WAIT_VSSIM_CORE_EXIT();

		TERM_IO_BUFFER();

		TERM_MAPPING_TABLE(); /* Term mapping -> Term core */

		TERM_VSSIM_CORE();
		TERM_FLASH_INFO();

		TERM_PERF_CHECKER();

#ifdef MONITOR_ON
		TERM_LOG_MANAGER();
#endif

		TERM_FLASH();
		printf("[%s] Total Write Pages : %ld\n", __FUNCTION__, GET_TOTAL_WRITE_PAGES());
		printf("[%s] Total Read Pages : %ld\n", __FUNCTION__, GET_TOTAL_READ_PAGES());
		printf("[%s] Total WRITE LPN : %ld\n", __FUNCTION__, GET_TOTAL_WRITE_LPN());
		printf("[%s] Total READ LPN : %ld\n", __FUNCTION__, GET_TOTAL_READ_LPN());
		printf("[%s] Total 5LC : %d\n", __FUNCTION__, GET_TOTAL_5LC());
		printf("[%s] Total 6LC : %d\n", __FUNCTION__, GET_TOTAL_6LC());
		printf("[%s] Total 7LC : %d\n", __FUNCTION__, GET_TOTAL_7LC());
		printf("[%s] Total 8LC : %d\n", __FUNCTION__, GET_TOTAL_8LC());
		printf("[%s] Average Bits Per Symbols  : %f\n", __FUNCTION__, GET_ENTROPY());
		printf("[%s] Compress Ratio  : %f\n", __FUNCTION__, GET_CR());
		printf("[%s] complete\n", __FUNCTION__);
		dump_total_writes();
	}
	pthread_mutex_unlock(&term_lock);

#ifdef GET_CH_UTIL_INFO
	fclose(fp_ch_util);
#endif
#ifdef GET_W_EVENT_INFO
	fclose(fp_w_event);
#endif
#ifdef GET_WB_LAT_INFO
	fclose(fp_wb_lat);
#endif
#ifdef GET_RW_LAT_INFO
	fclose(fp_rw_lat);
#endif

	return;
}

int FTL_READ(int core_id, uint64_t sector_nb, uint32_t length)
{
	volatile int ret;

	ret = _FTL_READ(core_id, sector_nb, length);
	if (ret == FAIL)
		printf("ERROR[%s] _FTL_READ function returns FAIL\n", __FUNCTION__);

	return ret;
}
static void calcFreq(unsigned char *str, int n)
{
	unsigned char Freq_8LC[256];
	for (int i = 0; i < 256; i++)
	{
		// DataBER_8LC[i] = 0;
		Freq_8LC[i] = 0;
	}
	unsigned char ch;
	// double BER = 0;   // wait to modify to meansure bit error rate
	for (int i = 0; i < n; i++)
	{
		ch = str[i];
		Freq_8LC[ch]++;
		// DataBER_8LC[ch] = BER;
		// BER = 0;
	}
	double probability;
	double average_symbol = 0;
	for (int i = 0; i < 256; i++)
	{
		if (Freq_8LC[i] != 0)
		{
			probability = (double)Freq_8LC[i] / (double)n;
			average_symbol += probability * log10(probability);
		}
	}
	if (overall_AverageSymbols == 0)
		overall_AverageSymbols += abs(average_symbol / log10(2));
	overall_AverageSymbols = (overall_AverageSymbols + abs(average_symbol / log10(2))) / 2;
}
static void dump_lba(uint64_t sector_nb, uint32_t length, volatile void *buf)
{
	mkdir("dump_lba", 0777);
	mkdir("dump_lba/5LC", 0777);
	mkdir("dump_lba/6LC", 0777);
	mkdir("dump_lba/7LC", 0777);
	mkdir("dump_lba/8LC", 0777);
	uint64_t lpn = sector_nb / (int64_t)SECTORS_PER_PAGE;
	unsigned int cur = length * SECTOR_SIZE;
	unsigned char *buffer;
	if (posix_memalign((void **)&buffer, 512, cur))
	{
		perror("posix_memalign failed");
		exit(1);
	}
	memcpy(buffer, buf, cur);
	char str[50];
	switch (get_DataAccessType(lpn))
	{
	//5LC
	case cWcR:
		sprintf(str, "dump_lba/5LC/%ld", sector_nb);
		break;
	//6LC
	case cWhR_tail:
		sprintf(str, "dump_lba/6LC/%ld", sector_nb);
		break;
	//6LC
	case cWhR_head:
		sprintf(str, "dump_lba/6LC/%ld", sector_nb);
		break;
	//8LC
	case hWcR:
		sprintf(str, "dump_lba/8LC/%ld", sector_nb);
		break;
	//7LC
	case hWhR:
		sprintf(str, "dump_lba/7LC/%ld", sector_nb);
		break;
	}
	int fd = open(str, O_WRONLY | O_CREAT, 0777);
	if (fd < 0)
	{
		perror("open tmp failed");
		free(buffer);
		return;
	}
	if (!write(fd, buffer, cur))
		perror("write tmp failed");
	close(fd);
	free(buffer);
}
static unsigned int ENCODE_REQ(void *buf, unsigned int length)
{
	unsigned char *buffer = NULL, *bufout = NULL;
	unsigned int bufoutlen = 0, cur = length * SECTOR_SIZE;
	buffer = (unsigned char *)malloc(cur);
	memcpy(buffer, buf, cur);
	calcFreq(buffer, cur);
	if (huffman_encode_memory(buffer, cur, &bufout, &bufoutlen))
		printf("encode fail\n");
	printf("\n\n[%s] origin buff : %s,len : %d\n", __FUNCTION__, buffer, cur);
	printf("[%s] compressd buff : %s,len : %d\n", __FUNCTION__, bufout, bufoutlen);
	printf("[%s] compressd ratio : %f.1\n", __FUNCTION__, 100 * (float)bufoutlen / (float)cur);
	if (compressratio == 0)
		compressratio += (float)bufoutlen / (float)cur;
	compressratio = (compressratio + (float)bufoutlen / (float)cur) / 2;
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
	if (huffman_decode_memory(bufout, bufoutlen, &buffer, &cur))
		printf("decode fail\n");
	printf("[%s] decompress buff : %s,len : %d\n", __FUNCTION__, buffer, cur);
	if (bufout != NULL)
	{
		free(bufout);
		bufout = NULL;
	}
	if (buffer != NULL)
	{
		free(buffer);
		buffer = NULL;
	}
	int tmp = bufoutlen % SECTOR_SIZE;
	bufoutlen += SECTOR_SIZE - tmp;
	printf("return %d\n\n", bufoutlen / SECTOR_SIZE);
	return (bufoutlen / SECTOR_SIZE);
}
int FTL_WRITE(volatile int core_id, volatile uint64_t sector_nb, volatile uint32_t length, volatile void *buf)
{
	/*
		Theory:
			1.	8LC Huffman encoding is nearly the same as binary Huffman coding.   log8   log2
			2.	8,7,6,5 are porportional to each other
			3.	Therefore, just using binary Huffman coding to emulate other encoding->(8,7,6,5 encoding)
		To Do List:
			1.	Hot Write/Hot Read LRU
			2.	Bit Error Rate
			3.	Cells Compress-Ratio
	*/
	volatile int n_pages;
	/*
		Added page buffer write func : _FTL_WRITE_WITH_PAGE_BUF
	*/
	// n_pages = _FTL_WRITE(core_id, sector_nb, length);
	// n_pages = _FTL_WRITE(core_id, sector_nb, recalculate_length(sector_nb,length));

	dump_lba(sector_nb, length, buf);

#ifdef ENCODE
	n_pages = _FTL_WRITE_WITH_PAGE_BUF(core_id, sector_nb, recalculate_length(sector_nb, ENCODE_REQ(buf, length)));
#else
	n_pages = _FTL_WRITE_WITH_PAGE_BUF(core_id, sector_nb, recalculate_length(sector_nb, length));
#endif
	if (n_pages == -1)
		printf("ERROR[%s] _FTL_WRITE function returns FAIL\n", __FUNCTION__);
	else
	{
		total_pages += n_pages;
		// printf("[%s] _FTL_WRITE write %d pages\n", __FUNCTION__, n_pages);
	}
	return n_pages;
}

void FTL_DISCARD(int core_id, uint64_t sector_nb, uint32_t length)
{
	if (sector_nb + length > N_SECTORS)
	{
		printf("ERROR[%s] Exceed Sector number\n", __FUNCTION__);
		return;
	}

#ifdef FTL_DEBUG
	printf("[%s] %d-core: Discard is called!\n", __FUNCTION__, core_id);
#endif

	uint64_t lba = sector_nb;
	int64_t lpn;
	int64_t lpn_4k;
	ppn_t ppn;
	pbn_t pbn;
	block_state_entry *bs_entry = NULL;
	uint32_t bitmap_index;

	uint32_t remain = length;
	uint32_t left_skip = sector_nb % SECTORS_PER_PAGE;

	int ret = FAIL;

	if (left_skip != 0 || (length % SECTORS_PER_4K_PAGE != 0))
	{
		printf("ERROR[%s] sector_nb: %lu, length: %u\n",
			   __FUNCTION__, sector_nb, length);
		return;
	}

	while (remain > 0)
	{

		/* Get the logical page number */
		lpn = lba / (int64_t)SECTORS_PER_PAGE;
		lpn_4k = lba / (int64_t)SECTORS_PER_4K_PAGE;

		/* Get the physical page number from the mapping table */
		ppn = GET_MAPPING_INFO(core_id, lpn);

		/* Get the block state entry of the ppn */
		pbn = PPN_TO_PBN(ppn);
		bs_entry = GET_BLOCK_STATE_ENTRY(pbn);

		/* Update bitmap */
		bitmap_index = (uint32_t)(lpn_4k % BITMAP_SIZE);

		ret = CLEAR_BITMAP(bs_entry->valid_array, bitmap_index);
		if (ret == FAIL)
		{
			return;
		}

		if (!TEST_BITMAP_MASK(bs_entry->valid_array, ppn.path.page))
		{
			bs_entry->n_valid_pages--;
		}

		lba += SECTORS_PER_4K_PAGE;
		remain -= SECTORS_PER_4K_PAGE;
		left_skip = 0;
	}

	return;
}

int _FTL_READ(int core_id, uint64_t sector_nb, uint32_t length)
{
#ifdef FTL_DEBUG
	printf("[%s] %d core: Start read %lu sector, %u length\n",
		   __FUNCTION__, core_id, sector_nb, length);
#endif

	if (sector_nb + length > N_SECTORS)
	{
		printf("Error[%s] Exceed Sector number\n", __FUNCTION__);
		return FAIL;
	}

	int64_t lpn;
	ppn_t ppn;
	uint64_t lba = sector_nb;
	uint32_t remain = length;
	uint32_t left_skip = sector_nb % SECTORS_PER_PAGE;
	uint32_t right_skip = 0;
	uint32_t read_sects;
	uint32_t n_trimmed_pages = 0;

	int n_pages = 0;
	int n_read_pages = 0;

	void *ret_buf = NULL;

#ifdef GET_CH_UTIL_INFO
	int n_ch_util = 0;
	double ch_util = 0;
#endif

	while (remain > 0)
	{

		if (remain > SECTORS_PER_PAGE - left_skip)
		{
			right_skip = 0;
		}
		else
		{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		read_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		lpn = lba / (int64_t)SECTORS_PER_PAGE;
		// LPN_FREQ * lpn_freq = (LPN_FREQ *)calloc(sizeof(LPN_FREQ), 1);
		// lpn_freq->lpn=lpn;
		// lpn_freq->prev=NULL;
		// lpn_freq->next=NULL;
		// insert_lru(lpn_freq,0);
		// if(lpn < 40000000)
		// {
		if (r_bloomfilter[lpn] < 128)
			r_bloomfilter[lpn]++;
		// }
		total_lpn_r++;
		ret_buf = CHECK_WRITE_BUFFER(core_id, lba, read_sects);

		if (ret_buf != NULL)
		{
			/* Hit Write Buffer */
		}
		else
		{
			/* Check Mapping Table */
			ppn = GET_MAPPING_INFO(core_id, lpn);

			if (ppn.addr != -1)
			{
				/* Read data from NAND page */
				FLASH_PAGE_READ(core_id, ppn);

				n_read_pages++;
			}
			else
			{
				/* Trimmed pages  */
				n_trimmed_pages++;
			}
		}
		n_pages++;

		ret_buf = NULL;
		lba += read_sects;
		remain -= read_sects;
		left_skip = 0;
	}

	/* Wait until all flash io are completed */
	WAIT_FLASH_IO(core_id, READ, n_read_pages);

#ifdef GET_CH_UTIL_INFO
	if (n_pages > vs_core[core_id].n_flash)
		n_ch_util = vs_core[core_id].n_flash;
	else
		n_ch_util = n_pages;

	ch_util = (double)n_ch_util / vs_core[core_id].n_flash;

	fprintf(fp_ch_util, "R\t%d\t%d\t%d\t%lf\n", core_id, n_ch_util, n_pages, ch_util);
#endif

#ifdef FTL_DEBUG
	printf("[%s] Complete\n", __FUNCTION__);
#endif

	/* If thie read request is for trimmed data, mark it to the core req entry */
	if (n_pages == n_trimmed_pages)
	{
		return TRIMMED;
	}
	total_page_read += n_pages;
	return SUCCESS;
}
int _FTL_WRITE(int core_id, uint64_t sector_nb, uint32_t length)
{
#ifdef FTL_DEBUG
	printf("[%s] %d core: Start write %lu sector, %u length\n",
		   __FUNCTION__, core_id, sector_nb, length);
#endif

	if (sector_nb + length > N_SECTORS)
	{
		printf("ERROR[%s] Exceed Sector number\n", __FUNCTION__);
		return -1;
	}

	uint64_t lba = sector_nb;
	int64_t lpn;
	ppn_t new_ppn;
	ppn_t old_ppn;
	pbn_t temp_pbn;

	uint32_t remain = length;
	uint32_t left_skip = sector_nb % SECTORS_PER_PAGE;
	uint32_t right_skip = 0;
	uint32_t write_sects;

	int ret = FAIL;
	int n_write_pages = 0;
	temp_pbn.addr = -1;

	while (remain > 0)
	{

		if (remain > SECTORS_PER_PAGE - left_skip)
		{
			right_skip = 0;
		}
		else
		{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}

		write_sects = SECTORS_PER_PAGE - left_skip - right_skip;

		ret = GET_NEW_PAGE(core_id, temp_pbn, MODE_OVERALL, &new_ppn, 0);
		if (ret == FAIL)
		{
			printf("ERROR[%s] Get new page fail \n", __FUNCTION__);
			return -1;
		}

#ifdef FTL_DEBUG
		printf("[%s] %d-core: get new page, f %d p %d b %d p %d (plane state: %d)\n",
			   __FUNCTION__, core_id, new_ppn.path.flash,
			   new_ppn.path.plane, new_ppn.path.block,
			   new_ppn.path.page,
			   flash_i[new_ppn.path.flash].plane_i[new_ppn.path.plane].p_state);
#endif

		lpn = lba / (int64_t)SECTORS_PER_PAGE;
		old_ppn = GET_MAPPING_INFO(core_id, lpn);
		if ((left_skip || right_skip) && (old_ppn.addr != -1))
		{
			//old request and (left or rigth) skip > 0
			// TEMP
			//			FLASH_PAGE_READ(core_id, old_ppn);
			//			WAIT_FLASH_IO(core_id, 1);

			FLASH_PAGE_WRITE(core_id, new_ppn);

			PARTIAL_UPDATE_PAGE_MAPPING(core_id, core_id, lpn, new_ppn,
										old_ppn, left_skip, right_skip);
		}
		else
		{
			ret = FLASH_PAGE_WRITE(core_id, new_ppn);

			UPDATE_OLD_PAGE_MAPPING(core_id, core_id, lpn);
			UPDATE_NEW_PAGE_MAPPING(core_id, lpn, new_ppn);
		}

		n_write_pages++;
		lba += write_sects;
		remain -= write_sects;
		left_skip = 0;
	}

#ifdef FTL_DEBUG
	printf("[%s] %d core: wait for writing %d pages\n",
		   __FUNCTION__, core_id, n_write_pages);
#endif

#ifdef FTL_DEBUG
	printf("[%s] %d core: End\n", __FUNCTION__, core_id);
#endif
	return n_write_pages;
}

int _FTL_WRITE_WITH_PAGE_BUF(volatile int core_id, volatile uint64_t sector_nb, volatile uint32_t length)
{
	if (sector_nb + length > N_SECTORS)
	{
		printf("ERROR[%s] Exceed Sector number\n", __FUNCTION__);
		return -1;
	}
	volatile uint64_t lba = sector_nb;
	volatile int64_t lpn;
	uint32_t remain = length;
	uint32_t left_skip = sector_nb % SECTORS_PER_PAGE;
	uint32_t right_skip = 0;
	uint32_t write_sects;
	int n_write_pages = 0;
	while (remain > 0)
	{
		if (remain > SECTORS_PER_PAGE - left_skip)
		{
			right_skip = 0;
		}
		else
		{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		write_sects = SECTORS_PER_PAGE - left_skip - right_skip;
		lpn = lba / (int64_t)SECTORS_PER_PAGE;
		LPN_INFO *lpn_info = (LPN_INFO *)calloc(sizeof(LPN_INFO), 1);
		lpn_info->lpn = lpn;
		lpn_info->prev = NULL;
		lpn_info->next = NULL;
		lpn_info->offset = write_sects;
		volatile int ret = FAIL;
		if (!INSERT_LPN_TO_PAGE_BUFFER(0, lpn_info))
			ret = SUCCESS;
		else if (!INSERT_LPN_TO_PAGE_BUFFER(1, lpn_info))
			ret = SUCCESS;
		if (ret == FAIL)
		{
			if (p_buffer[0].n_empty_space > p_buffer[1].n_empty_space)
			{
				FLUSH_PAGE_BUFFER(1, core_id);
				ret = INSERT_LPN_TO_PAGE_BUFFER(1, lpn_info);
			}
			else
			{
				FLUSH_PAGE_BUFFER(0, core_id);
				ret = INSERT_LPN_TO_PAGE_BUFFER(0, lpn_info);
			}
			n_write_pages++;
		}
		if (ret == FAIL)
			printf("ERROR[%s] After flush, still fail\n", __FUNCTION__);
		lba += write_sects;
		remain -= write_sects;
		left_skip = 0;
	}
	return n_write_pages;
}
void INIT_PAGE_BUFFER(void)
{
	int i;

	/* Allocate write buffer structure */
	p_buffer = (page_buffer *)calloc(sizeof(page_buffer), 2);
	if (p_buffer == NULL)
	{
		printf("ERROR[%s] Allocation page buffer structure fail!\n", __FUNCTION__);
		return;
	}

	for (i = 0; i < 2; i++)
	{
		/* Initialize write buffer structure */
		p_buffer[i].head = NULL;
		p_buffer[i].tail = NULL;
		p_buffer[i].n_empty_space = SECTORS_PER_PAGE;
		// p_buffer[i].ftl_ptr = vssim_w_buf[i].addr;
		// p_buffer[i].host_ptr = vssim_w_buf[i].addr;
		// p_buffer[i].buffer_end = vssim_w_buf[i].addr
		// 		+ N_WB_SECTORS*SECTOR_SIZE;
		p_buffer[i].is_full = 0;
		// p_buffer[i].t_last_flush = 0;
		p_buffer[i].is_empty = 1;
		pthread_mutex_init(&p_buffer[i].lock, NULL);
		pthread_cond_init(&p_buffer[i].ready, NULL);
		printf("[%s] p_buffer %d.%d\n", __FUNCTION__, i, p_buffer[i].n_empty_space);
	}
}
int INSERT_LPN_TO_PAGE_BUFFER(int buffer_nb, LPN_INFO *cur)
{
	/* Get the out event queue lock */
	pthread_mutex_lock(&p_buffer[buffer_nb].lock);
	if (p_buffer[buffer_nb].n_empty_space < cur->offset)
	{
		// #ifdef PAGE_BUFFER_DEBUG
		// 		printf("[%s] page buffer %d is full\n", __FUNCTION__, buffer_nb);
		// #endif
		pthread_mutex_unlock(&p_buffer[buffer_nb].lock);
		return FAIL;
	}
	/* Insert new out event entry */
	if (p_buffer[buffer_nb].n_empty_space == SECTORS_PER_PAGE)
	{
		p_buffer[buffer_nb].head = cur;
		p_buffer[buffer_nb].tail = cur;
	}
	else
	{
		p_buffer[buffer_nb].tail->next = cur;
		cur->prev = p_buffer[buffer_nb].tail;
		p_buffer[buffer_nb].tail = cur;
	}
	/* Increase the number of out event entry number */
	p_buffer[buffer_nb].n_empty_space -= cur->offset;
#ifdef PAGE_BUFFER_DEBUG
	printf("[%s] page buffer %d insert lpn: %ld, remain %d sectors\n", __FUNCTION__, buffer_nb, p_buffer[buffer_nb].tail->lpn, p_buffer[buffer_nb].n_empty_space);
#endif
	/* Release the out event queue lock */
	pthread_mutex_unlock(&p_buffer[buffer_nb].lock);
	return SUCCESS;
}
void FLUSH_PAGE_BUFFER(int buffer_nb, int core_id)
{
	// #ifdef PAGE_BUFFER_DEBUG
	// printf("[%s] page buffer %d is flushed \n", __FUNCTION__, buffer_nb);
	// #endif
	LPN_INFO *cur;
	ppn_t new_ppn;
	pbn_t temp_pbn;
	volatile int ret = FAIL;

	ret = GET_NEW_PAGE(core_id, temp_pbn, MODE_OVERALL, &new_ppn, 0);
	if (ret == FAIL)
	{
#ifdef PAGE_BUFFER_DEBUG
		printf("ERROR[%s] Get new page fail \n", __FUNCTION__);
#endif
	}
	// #ifdef PAGE_BUFFER_DEBUG
	// 	printf("[%s] %d-core: get new page, f %d p %d b %d p %d (plane state: %d)\n",
	// 		   __FUNCTION__, core_id, new_ppn.path.flash,
	// 		   new_ppn.path.plane, new_ppn.path.block,
	// 		   new_ppn.path.page,
	// 		   flash_i[new_ppn.path.flash].plane_i[new_ppn.path.plane].p_state);
	// #endif
	FLASH_PAGE_WRITE(core_id, new_ppn);

	pthread_mutex_lock(&p_buffer[buffer_nb].lock);

	while (p_buffer[buffer_nb].head != NULL)
	{
		cur = p_buffer[buffer_nb].head;
		// #ifdef PAGE_BUFFER_DEBUG
		// 		printf("[%s] current lpn : %ld\n", __FUNCTION__, cur->lpn);
		// #endif
		p_buffer[buffer_nb].head = cur->next;

		UPDATE_OLD_PAGE_MAPPING(core_id, core_id, cur->lpn);
		UPDATE_NEW_PAGE_MAPPING(core_id, cur->lpn, new_ppn);

		p_buffer[buffer_nb].n_empty_space += cur->offset;
		// #ifdef PAGE_BUFFER_DEBUG
		// 		printf("[%s] lpn %ld is flushed in page buffer %d, now remain %d sectors\n", __FUNCTION__, cur->lpn, buffer_nb, p_buffer[buffer_nb].n_empty_space);
		// #endif
		free(cur);
	}
#ifdef PAGE_BUFFER_DEBUG
	printf("[%s] After flush page buffer %d, now remain %d sectors\n", __FUNCTION__, buffer_nb, p_buffer[buffer_nb].n_empty_space);
#endif
	pthread_mutex_unlock(&p_buffer[buffer_nb].lock);
}
void INIT_HOT_LRU(void)
{
	/* Allocate write buffer structure */
	hot_r_lru = (HOT_LRU *)calloc(sizeof(HOT_LRU), 1);
	hot_w_lru = (HOT_LRU *)calloc(sizeof(HOT_LRU), 1);
	if (hot_r_lru == NULL && hot_w_lru == NULL)
	{
		printf("ERROR[%s] Allocation page buffer structure fail!\n", __FUNCTION__);
		return;
	}
	hot_r_lru->head = NULL;
	hot_w_lru->tail = NULL;
	hot_r_lru->head = NULL;
	hot_w_lru->tail = NULL;
	hot_r_lru->length = 0;
	hot_w_lru->length = 0;
	pthread_mutex_init(&hot_r_lru->lock, NULL);
	pthread_mutex_init(&hot_w_lru->lock, NULL);
}
uint32_t recalculate_length(uint64_t sector_nb, uint32_t length)
{
	if (sector_nb + length > N_SECTORS)
	{
		printf("ERROR[%s] Exceed Sector number\n", __FUNCTION__);
		return -1;
	}
	uint64_t lba = sector_nb;
	int64_t lpn;
	uint32_t remain = length;
	uint32_t left_skip = sector_nb % SECTORS_PER_PAGE;
	uint32_t right_skip = 0;
	uint32_t write_sects, write_sects_tmp;
	// float scale = 1;
	uint32_t length_scale = length;
	while (remain > 0)
	{
		if (remain > SECTORS_PER_PAGE - left_skip)
		{
			right_skip = 0;
		}
		else
		{
			right_skip = SECTORS_PER_PAGE - left_skip - remain;
		}
		write_sects = SECTORS_PER_PAGE - left_skip - right_skip;
		write_sects_tmp = write_sects;
		lpn = lba / (int64_t)SECTORS_PER_PAGE;
		total_lpn_w++;
		// switch (cWhR_head)
		// switch (hWhR)
		switch (get_DataAccessType(lpn))
		{
		//5LC
		case cWcR:
// scale*=1.29;
#ifdef ENCODE
			write_sects_tmp *= 1.29;
			length_scale += write_sects_tmp - write_sects;
#endif
			total_5LC++;
			printf("[%s] LPN %ld is cWcR\n", __FUNCTION__, lpn);
			break;
		//6LC
		case cWhR_tail:
// scale*=1.16;
#ifdef ENCODE
			write_sects_tmp *= 1.16;
			length_scale += write_sects_tmp - write_sects;
#endif
			total_6LC++;
			printf("[%s] LPN %ld is cWhR_tail\n", __FUNCTION__, lpn);
			break;
		//6LC
		case cWhR_head:
// scale*=1.16;
#ifdef ENCODE
			write_sects_tmp *= 1.16;
			length_scale += write_sects_tmp - write_sects;
#endif
			total_6LC++;
			printf("[%s] LPN %ld is cWhR_head\n", __FUNCTION__, lpn);
			break;
		//8LC
		case hWcR:
			total_8LC++;
			printf("[%s] LPN %ld is hWcR\n", __FUNCTION__, lpn);
			break;
		//7LC
		case hWhR:
// scale*=1.068;
#ifdef ENCODE
			write_sects_tmp *= 1.068;
			length_scale += write_sects_tmp - write_sects;
#endif
			total_7LC++;
			printf("[%s] LPN %ld is hWhR\n", __FUNCTION__, lpn);
			break;
		}
		// if(lpn < 40000000)
		// {
		if (w_bloomfilter[lpn] < 128)
			w_bloomfilter[lpn]++;
		// }
		// LPN_FREQ * lpn_freq = (LPN_FREQ *)calloc(sizeof(LPN_FREQ), 1);
		// lpn_freq->lpn=lpn;
		// lpn_freq->prev=NULL;
		// lpn_freq->next=NULL;
		// insert_lru(lpn_freq,1);
		lba += write_sects;
		remain -= write_sects;
		left_skip = 0;
	}
// printf("[%s] all 8LC length : %d, recalculate length : %d, scale : %f\n", __FUNCTION__,length,(uint32_t)((double)length * (double)scale),scale);
// return (double)length * (double)scale;
#ifdef ENCODE_SCALE
	return length_scale;
#else
	return length;
#endif
}
int get_DataAccessType(int64_t lpn)
{
	uint8_t th_w = HOT_W_THRESHOLD;
	uint8_t th_r = HOT_R_THRESHOLD;
	printf("[%s] LPN %ld update times : %d, read times : %d, total lpn writes : %ld\n", __FUNCTION__, lpn, w_bloomfilter[lpn], r_bloomfilter[lpn], GET_TOTAL_WRITE_LPN());
	if (w_bloomfilter[lpn] != 0)
		total_updates++;
	if (w_bloomfilter[lpn] > th_w)
	{
		if (r_bloomfilter[lpn] > th_r)
			return hWhR;
		else
			return hWcR;
	}
	else
	{
		if (r_bloomfilter[lpn] > th_r)
			if (r_bloomfilter[lpn] > th_r * 2)
				return cWhR_head;
			else
				return cWhR_tail;
		else
			return cWcR;
	}
	// int index_w, index_r;
	// index_w = check_lru_index(lpn,1);
	// index_r = check_lru_index(lpn,0);
	// if(index_w>90)
	// {
	// 	if(index_r>50)
	// 		return hWhR;
	// 	else
	// 		return hWcR;
	// }
	// else
	// {
	// 	if(index_r>50)
	// 		if(index_r>75)
	// 			return cWhR_head;
	// 		else
	// 			return cWhR_tail;
	// 	else
	// 		return cWcR;
	// }
}
int check_lru_index(int64_t lpn, int type)
{
	pthread_mutex_lock(&hot_r_lru->lock);
	pthread_mutex_lock(&hot_w_lru->lock);
	volatile HOT_LRU *lru_type;
	if (type)
		lru_type = hot_w_lru;
	else
		lru_type = hot_r_lru;
	uint64_t length = lru_type->length;
	if (!length)
	{
		pthread_mutex_unlock(&hot_r_lru->lock);
		pthread_mutex_unlock(&hot_w_lru->lock);
		return 0;
	}
	LPN_FREQ *tmp;
	tmp = lru_type->head;
	volatile int64_t i = 0;
	while (tmp != NULL)
	{
		if (tmp->lpn == lpn)
		{
			// printf("[%s] Find LPN in Hot LRU\n", __FUNCTION__);
			pthread_mutex_unlock(&hot_r_lru->lock);
			pthread_mutex_unlock(&hot_w_lru->lock);
			printf("[%s] LPN %ld, index :%ld, total LRU length :%ld\n", __FUNCTION__, lpn, i, length);
			return i * 100 / length;
		}
		tmp = tmp->next;
		i++;
	}
	// printf("[%s] No Find LPN in Hot LRU\n", __FUNCTION__);
	pthread_mutex_unlock(&hot_r_lru->lock);
	pthread_mutex_unlock(&hot_w_lru->lock);
	return 0;
}
// void insert_lru(uint64_t new_lpn,int type)
// {
// 	pthread_mutex_lock(&hot_r_lru->lock);
// 	pthread_mutex_lock(&hot_w_lru->lock);
// 	volatile HOT_LRU * lru_type;
// 	if(type)
// 		lru_type = hot_w_lru;
// 	else
// 		lru_type = hot_r_lru;
// 	/* Insert new out event entry */
// 	if(lru_type->length == 0){
// 		lru_type->head = new_lpn;
// 		lru_type->tail = new_lpn;
// 		new_lpn->freq++;
// 		lru_type->length++;
// 	}
// 	else{
// 		LPN_FREQ *tmp;
// 		tmp=lru_type->head;
// 		new_lpn->freq++;
// 		while(tmp->next!=NULL)
// 		{
// 			if(tmp->lpn==new_lpn->lpn){
// 				free(new_lpn);
// 				// printf("[%s] Find LPN in Hot LRU\n", __FUNCTION__);
// 				new_lpn=tmp;
// 				new_lpn->freq++;
// 				if(new_lpn==lru_type->head)
// 				{
// 					lru_type->head = new_lpn->next;
// 					new_lpn->next->prev=NULL;
// 				}else{
// 					tmp->prev->next = tmp->next;
// 					tmp->next->prev = tmp->prev;
// 				}
// 				lru_type->length--;
// 				break;
// 			}
// 			tmp=tmp->next;
// 		}
// 		// if(new_lpn->freq<2)
// 		// 	printf("[%s] No Find LPN in Hot LRU, starting insertion\n", __FUNCTION__);
// 		lru_type->tail->next = new_lpn;
// 		new_lpn->prev = lru_type->tail;
// 		lru_type->tail = new_lpn;
// 		lru_type->tail->next = NULL;
// 		lru_type->length++;
// 	}
// 	LPN_FREQ *tmp;
// 	tmp=lru_type->tail;
// 	int i=0;
// 	while(tmp!=NULL && i++<255)
// 	{
// 		// printf("%d, ",tmp->freq);
// 		tmp=tmp->prev;
// 	}
// 	// printf("\n[%s]LPN : %ld\nTotal Count : %d\nCurrent HOT LRU length %ld, type : %d\n", __FUNCTION__,new_lpn->lpn,i,lru_type->length, type);
// 	pthread_mutex_unlock(&hot_r_lru->lock);
// 	pthread_mutex_unlock(&hot_w_lru->lock);
// }
void insert_lru(LPN_FREQ *new_lpn, int type)
{
	pthread_mutex_lock(&hot_r_lru->lock);
	pthread_mutex_lock(&hot_w_lru->lock);
	volatile HOT_LRU *lru_type;
	if (type)
		lru_type = hot_w_lru;
	else
		lru_type = hot_r_lru;
	/* Insert new out event entry */
	if (lru_type->length == 0)
	{
		lru_type->head = new_lpn;
		lru_type->tail = new_lpn;
		new_lpn->freq++;
		lru_type->length++;
	}
	else
	{
		LPN_FREQ *tmp;
		tmp = lru_type->head;
		new_lpn->freq++;
		while (tmp->next != NULL)
		{
			if (tmp->lpn == new_lpn->lpn)
			{
				free(new_lpn);
				// printf("[%s] Find LPN in Hot LRU\n", __FUNCTION__);
				new_lpn = tmp;
				new_lpn->freq++;
				if (new_lpn == lru_type->head)
				{
					lru_type->head = new_lpn->next;
					new_lpn->next->prev = NULL;
				}
				else
				{
					tmp->prev->next = tmp->next;
					tmp->next->prev = tmp->prev;
				}
				lru_type->length--;
				break;
			}
			tmp = tmp->next;
		}
		// if(new_lpn->freq<2)
		// 	printf("[%s] No Find LPN in Hot LRU, starting insertion\n", __FUNCTION__);
		lru_type->tail->next = new_lpn;
		new_lpn->prev = lru_type->tail;
		lru_type->tail = new_lpn;
		lru_type->tail->next = NULL;
		lru_type->length++;
	}
	LPN_FREQ *tmp;
	tmp = lru_type->tail;
	int i = 0;
	while (tmp != NULL && i++ < 255)
	{
		// printf("%d, ",tmp->freq);
		tmp = tmp->prev;
	}
	// printf("\n[%s]LPN : %ld\nTotal Count : %d\nCurrent HOT LRU length %ld, type : %d\n", __FUNCTION__,new_lpn->lpn,i,lru_type->length, type);
	pthread_mutex_unlock(&hot_r_lru->lock);
	pthread_mutex_unlock(&hot_w_lru->lock);
}
unsigned long int GET_TOTAL_WRITE_PAGES(void)
{
	return total_pages;
}
unsigned long int GET_TOTAL_READ_PAGES(void)
{
	return total_page_read;
}
uint64_t GET_TOTAL_WRITE_LPN(void)
{
	return total_lpn_w;
}
uint64_t GET_TOTAL_READ_LPN(void)
{
	return total_lpn_r;
}
uint32_t GET_TOTAL_5LC(void)
{
	return total_5LC;
}
uint32_t GET_TOTAL_6LC(void)
{
	return total_6LC;
}
uint32_t GET_TOTAL_7LC(void)
{
	return total_7LC;
}
uint32_t GET_TOTAL_8LC(void)
{
	return total_8LC;
}
double GET_ENTROPY(void)
{
	return overall_AverageSymbols;
}
double GET_CR(void)
{
	return compressratio * 100;
}
// LPN_FREQ* reverseList(LPN_FREQ* head)
// {
//         if(head==NULL)
// 			return head;
//         LPN_FREQ* tempHead = head;
//         LPN_FREQ* prev = NULL;
//         while(tempHead->next){
//             LPN_FREQ* cur = tempHead;
//             tempHead = tempHead->next;
//             cur->next = prev;
//             prev = cur;
//         }
//         tempHead->next = prev;
//         return tempHead;
// }
// LPN_FREQ* insertionSortList(LPN_FREQ* head)
// {
//     // insertion sort
//     if(!head)
// 		return NULL;
//     LPN_FREQ* temp = head;

//     // get size first
//     int size = 0;
//     while(temp) {
//         size++;
//         temp = temp->next;
//     }
//     // curr is the next element of sorted list
//     LPN_FREQ* curr = head->next;
//     // prev->next == curr
//     LPN_FREQ* prev = head;
//     // tail indicates the tail of sorted list
//     LPN_FREQ* tail = head;

//     for(int i=1;i<size;i++) {
//         temp = head;
//         prev = head;
//         // find the location to be inserted
//         for(int j=0;j<i && temp->freq < curr->freq;j++) {
//             temp = temp->next;
//             if(j!=0)
// 				prev = prev->next;
//         }
//         // insert
//         if(temp==head) {
//             tail->next = curr->next;
//             curr->next = head;
//             head = curr;
//         } else if(temp==curr) {
//             tail = tail->next;
//         } else {
//             prev->next = curr;
//             tail->next = curr->next;
//             curr->next = temp;
//         }
//         curr = tail->next;
//     }
//     return reverseList(head);
// }