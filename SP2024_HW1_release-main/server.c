#include "server.h"

const unsigned char IAC_IP[3] = "\xff\xf4";
const char* file_prefix = "./csie_trains/train_";
const char* accept_read_header = "ACCEPT_FROM_READ";
const char* accept_write_header = "ACCEPT_FROM_WRITE";
const char* welcome_banner = "======================================\n"
                             " Welcome to CSIE Train Booking System \n"
                             "======================================\n";

const char* lock_msg = ">>> Locked.\n";
const char* exit_msg = ">>> Client exit.\n";
const char* cancel_msg = ">>> You cancel the seat.\n";
const char* full_msg = ">>> The shift is fully booked.\n";
const char* seat_booked_msg = ">>> The seat is booked.\n";
const char* no_seat_msg = ">>> No seat to pay.\n";
const char* book_succ_msg = ">>> Your train booking is successful.\n";
const char* invalid_op_msg = ">>> Invalid operation.\n";

#ifdef READ_SERVER
char* read_shift_msg = "Please select the shift you want to check [902001-902005]: ";
#elif defined WRITE_SERVER
char* write_shift_msg = "Please select the shift you want to book [902001-902005]: ";
char* write_seat_msg = "Select the seat [1-40] or type \"pay\" to confirm: ";
char* write_seat_or_exit_msg = "Type \"seat\" to continue or \"exit\" to quit [seat/exit]: ";
#endif

static void init_server(unsigned short port);
// initailize a server, exit for error

static void init_request(request* reqP);
// initailize a request instance

static void init_record(record* recP);
// initialize a record instance

static void free_request(request* reqP);
// free resources used by a request instance

static void free_record(record* recP);

int accept_conn(void);
// accept connection

static void getfilepath(char* filepath, int extension);
// get record filepath

int8_t occupied(int32_t fd){
	/* Return value:
	 * 1: is fully occupied
	 * 0: isn't fully occupied
	 * -1: read failed
	 */
	char tbuf[1024];
	int32_t ret = read(fd, tbuf, sizeof(tbuf) - 1);
	if(ret < 0){
		return -1;
	}
	lseek(fd, 0, SEEK_SET);
	tbuf[ret] = '\0';
	for(int i = 0;tbuf[i] != '\0';i++){
		if(tbuf[i] == '0') return 0;
	}
	return 1;
}

int handle_read(request* reqP) {
    /*  Return value:
     *      1: read successfully
     *      0: read EOF (client down)
     *     -1: read failed
     *   TODO: handle incomplete input
     */
    int r;
    char buf[MAX_MSG_LEN];
    size_t len;

    memset(buf, 0, sizeof(buf));

    // Read in request from client
    r = read(reqP->conn_fd, buf, sizeof(buf));
    if (r < 0) return -1;
    if (r == 0) return 0;
    char* p1 = strstr(buf, "\015\012"); // \r\n
    if (p1 == NULL) {
        p1 = strstr(buf, "\012");   // \n
        if (p1 == NULL) {
            if (!strncmp(buf, IAC_IP, 2)) {
                // Client presses ctrl+C, regard as disconnection
                fprintf(stderr, "Client presses ctrl+C....\n");
                return 0;
            }
        }
    }

    len = p1 - buf + 1;
    memmove(reqP->buf, buf, len);
    reqP->buf[len - 1] = '\0';
    reqP->buf_len = len-1;
    return 1;
}

#ifdef READ_SERVER
int print_train_info(request *reqP) {
    
    int i;
    char buf[MAX_MSG_LEN];

    memset(buf, 0, sizeof(buf));
    for (i = 0; i < SEAT_NUM / 4; i++) {
        sprintf(buf + (i * 4 * 2), "%d %d %d %d\n", 0, 0, 0, 0);
    }
    return 0;
}
#else
int print_train_info(request *reqP, record *recP) {
    /*
     * Booking info
     * |- Shift ID: 902001
     * |- Chose seat(s): 1,2
     * |- Paid: 3,4
     */
    char buf[MAX_MSG_LEN*3];
    char chosen_seat[MAX_MSG_LEN] = {'\0'};
    char paid[MAX_MSG_LEN] = {'\0'};

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "\nBooking info\n"
                 "|- Shift ID: %d\n"
                 "|- Chose seat(s): %s\n"
                 "|- Paid: %s\n\n"
                 ,recP->shift_id, chosen_seat, paid);
	write(reqP->conn_fd, buf, strlen(buf));
    return 0;
}
#endif

int main(int argc, char** argv) {

    // Parse args.
    if (argc != 2) {
        fprintf(stderr, "usage: %s [port]\n", argv[0]);
        exit(1);
    }

    int conn_fd;  // fd for file that we open for reading
    char buf[MAX_MSG_LEN*2], filename[FILE_LEN];

    int i,j;

    for (i = TRAIN_ID_START, j = 0; i <= TRAIN_ID_END; i++, j++) {
        getfilepath(filename, i);
#ifdef READ_SERVER
        trains[j].file_fd = open(filename, O_RDONLY);
#elif defined WRITE_SERVER
        trains[j].file_fd = open(filename, O_RDWR);
#else
        trains[j].file_fd = -1;
#endif
        if (trains[j].file_fd < 0) {
            ERR_EXIT("open");
        }
    }

    // Initialize server
    init_server((unsigned short) atoi(argv[1]));

    // Loop for handling connections
    fprintf(stderr, "\nstarting on %.80s, port %d, fd %d, maxconn %d...\n", svr.hostname, svr.port, svr.listen_fd, maxfd);
	int32_t flag = fcntl(svr.listen_fd, F_GETFL, 0);
	fcntl(svr.listen_fd, F_SETFL, flag | O_NONBLOCK);
	
	fd_set read_fds, all_fds;
	FD_ZERO(&all_fds);
	FD_SET(svr.listen_fd, &all_fds);
    while(1){
        // TODO: Add IO multiplexing
        // Check new connection
		FD_ZERO(&read_fds);
		read_fds = all_fds;
		for(int i = 0;i < maxfd;i++){
			if(requestP[i].conn_fd != -1){
				FD_SET(requestP[i].conn_fd, &read_fds);
			}
		}
		int32_t activity = 0;
#ifdef READ_SERVER
		activity = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
#elif defined WRITE_SERVER
		activity = select(maxfd + 1, &read_fds, NULL, NULL, NULL);
#endif
		if(activity < 0){
			ERR_EXIT("select failed.");
		}
		if(FD_ISSET(svr.listen_fd, &read_fds)){
			conn_fd = accept_conn();
			if(conn_fd >= 0){
				write(requestP[conn_fd].conn_fd, welcome_banner, strlen(welcome_banner));
#ifdef READ_SERVER
				write(requestP[conn_fd].conn_fd, read_shift_msg, strlen(read_shift_msg));
#elif defined WRITE_SERVER
				write(requestP[conn_fd].conn_fd, write_shift_msg, strlen(write_shift_msg));
#endif
			}
		}
        // TODO: handle requests from clients
		for(int i = 0;i < maxfd;i++){
			int client_fd = requestP[i].conn_fd;
			if(client_fd != -1 && FD_ISSET(client_fd, &read_fds)){
				int ret = handle_read(&requestP[i]);
				if(ret < 0){
					fprintf(stderr, "bad request from %s\n", requestP[i].host);
					continue;
				}
				if(ret == 0){
					close(requestP[i].conn_fd);
					free_request(&requestP[i]);
					break;
				}
#ifdef READ_SERVER      
				sprintf(buf,"%s : %s\n",accept_read_header,requestP[i].buf);
				write(requestP[i].conn_fd, buf, strlen(buf));
				char trainInfo[1024];
				int readTrainNum = atoi(requestP[conn_fd].buf) - TRAIN_ID_START;
				// printf("client file descriptor: %d\n", i);
				ret = read(trains[readTrainNum].file_fd, trainInfo, sizeof(trainInfo) - 1);
				if(ret < 0){
					fprintf(stderr, "Failed to read %s\n", atoi(requestP[i].buf));
					close(requestP[i].conn_fd);
					free_request(&requestP[i]);
					break;
				}
				lseek(trains[readTrainNum].file_fd, 0, SEEK_SET);
				trainInfo[ret] = '\0';
				ret = write(requestP[i].conn_fd, trainInfo, strlen(trainInfo));
				write(requestP[conn_fd].conn_fd, read_shift_msg, strlen(read_shift_msg));
#elif defined WRITE_SERVER
				sprintf(buf,"%s : %s\n",accept_write_header,requestP[i].buf);
				write(requestP[i].conn_fd, buf, strlen(buf)); 
				char trainInfo[1024];
				int readTrainNum = atoi(requestP[conn_fd].buf) - TRAIN_ID_START;
				if(occupied(trains[readTrainNum].file_fd)){
					ret = write(requestP[i].conn_fd, full_msg, strlen(full_msg));
					if(ret < 0){
						fprintf(stderr, "Failed to read %s\n", atoi(requestP[i].buf));
						close(requestP[i].conn_fd);
						free_request(&requestP[i]);
						break;
					}
					write(requestP[conn_fd].conn_fd, write_shift_msg, strlen(write_shift_msg));
				}
				else if(occupied(trains[readTrainNum].file_fd) == 0){
					recordP[i].shift_id = atoi(requestP[i].buf);
					print_train_info(&requestP[i], &recordP[i]);
					write(requestP[i].conn_fd, write_seat_msg, strlen(write_seat_msg));
				}
				else if(occupied(trains[readTrainNum].file_fd) == -1){
					fprintf(stderr, "Failed to read %s\n", atoi(requestP[i].buf));
					close(requestP[i].conn_fd);
					free_request(&requestP[i]);
					break;
				}
#endif
			}
		}
        // close(requestP[conn_fd].conn_fd);
        // free_request(&requestP[conn_fd]);
    }      

    free(requestP);
    close(svr.listen_fd);
    for (i = 0;i < TRAIN_NUM; i++)
        close(trains[i].file_fd);

    return 0;
}

int accept_conn(void) {

    struct sockaddr_in cliaddr;
    size_t clilen;
    int conn_fd;  // fd for a new connection with client

    clilen = sizeof(cliaddr);
    conn_fd = accept(svr.listen_fd, (struct sockaddr*)&cliaddr, (socklen_t*)&clilen);
    if(conn_fd < 0){
        if (errno == EINTR || errno == EAGAIN) return -1;  // try again
        if (errno == ENFILE) {
            (void) fprintf(stderr, "out of file descriptor table ... (maxconn %d)\n", maxfd);
                return -1;
        }
        ERR_EXIT("accept");
    }
    
    requestP[conn_fd].conn_fd = conn_fd;
    strcpy(requestP[conn_fd].host, inet_ntoa(cliaddr.sin_addr));
    fprintf(stderr, "getting a new request... fd %d from %s\n", conn_fd, requestP[conn_fd].host);
    requestP[conn_fd].client_id = (svr.port * 1000) + num_conn;    // This should be unique for the same machine.
    num_conn++;
    
    return conn_fd;
}

static void getfilepath(char* filepath, int extension) {
    char fp[FILE_LEN*2];
    
    memset(filepath, 0, FILE_LEN);
    sprintf(fp, "%s%d", file_prefix, extension);
    strcpy(filepath, fp);
}

// ======================================================================================================
// You don't need to know how the following codes are working
#include <fcntl.h>

static void init_request(request* reqP) {
    reqP->conn_fd = -1;
    reqP->client_id = -1;
    reqP->buf_len = 0;
    reqP->status = INVALID;
    reqP->remaining_time.tv_sec = 5;
    reqP->remaining_time.tv_usec = 0;

    reqP->booking_info.num_of_chosen_seats = 0;
    reqP->booking_info.train_fd = -1;
    for (int i = 0; i < SEAT_NUM; i++)
        reqP->booking_info.seat_stat[i] = UNKNOWN;
}

static void init_record(record* recP){
	recP->shift_id = -1;
	recP->train_fd = -1;
	recP->num_of_chosen_seats = 0;
	for(int i = 0;i < SEAT_NUM;i++){
		recP->seat_stat[i] = UNKNOWN;
	}
}

static void free_record(record* recP){
	memset(recP, 0, sizeof(record));
	init_record(recP);
}

static void free_request(request* reqP) {
    memset(reqP, 0, sizeof(request));
    init_request(reqP);
}

static void init_server(unsigned short port) {
    struct sockaddr_in servaddr;
    int tmp;

    gethostname(svr.hostname, sizeof(svr.hostname));
    svr.port = port;

    svr.listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (svr.listen_fd < 0) ERR_EXIT("socket");

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);
    tmp = 1;
    if (setsockopt(svr.listen_fd, SOL_SOCKET, SO_REUSEADDR, (void*)&tmp, sizeof(tmp)) < 0) {
        ERR_EXIT("setsockopt");
    }
    if (bind(svr.listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        ERR_EXIT("bind");
    }
    if (listen(svr.listen_fd, 1024) < 0) {
        ERR_EXIT("listen");
    }

    // Get file descripter table size and initialize request table
    maxfd = getdtablesize();
    requestP = (request*) malloc(sizeof(request) * maxfd);
    if(requestP == NULL){
        ERR_EXIT("out of memory allocating all requests");
    }
	recordP = (record*)malloc(sizeof(record) * maxfd);
	if(recordP == NULL){
		ERR_EXIT("out of memory allocating all requests");
	}
    for(int i = 0; i < maxfd; i++){
        init_request(&requestP[i]);
		init_record(&recordP[i]);
    }
    requestP[svr.listen_fd].conn_fd = svr.listen_fd;
    strcpy(requestP[svr.listen_fd].host, svr.hostname);

    return;
}
