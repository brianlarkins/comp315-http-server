/*
 * httpserver.c - a simple HTTP server program
 * author:
 *
 */

#include "httpserver.h"


/*
 * global variables
 */
extension_t mimetypes[MAX_MIMETYPES];
int ecount = 0;
char *def_obj = "/index.html";



/*
 * parse_req - parses and validates an HTTP request
 *
 * @param buf buffer containing a full request from the client
 * @param req a request object filled out by this function
 * @returns a status code corresponding to the validity of the request
 *
 */
status_t parse_req(char *buf, request_t *req) {
  status_t rc = OK;

  /* implement me */

  return rc;
}



/*
 * send_reply - sends a reply to the client based on the requested resource
 *
 * @param cfd client socket file descriptor
 * @param req request object completed by parse_req
 * @param wwwpath path to the directory containing the web documents
 *
 */
void send_reply(int cfd, status_t sc, request_t *req, char *wwwpath) {

  /* implement me */

}



/*
 * get_req - reads a complete HTTP request into a buffer
 *
 * @param cfd client socket file descriptor
 * @param buf buffer to read request into
 * @returns RecvOK if successful, RecvTimeout on timeout, RecvError on error
 *
 */
connstatus_t get_req(int cfd, char *buf) {
  int nbytes, rbytes, remaining;
  connstatus_t rv = RecvOK;

  rbytes = 0;
  remaining = BUFSIZE;

  do {
    nbytes = recv(cfd, &buf[rbytes], remaining, 0);
    if ((nbytes == -1) && (errno == EAGAIN)) {
      rv = RecvTimeout;
      goto done;

    } else if (nbytes == -1) {
      perror("recv");
      rv = RecvError;
      goto done;
    }

    rbytes += nbytes;
    remaining -= nbytes;

  } while (check_buf(buf, rbytes));

done:
  return rv;
}



/*
 * handle_req - handles a one or more HTTP requst/reply pairs with a client
 *
 * @param cfd client socket file descriptor
 * @param wwwpath path to the directory containing the web documents
 *
 */
void handle_req(int cfd, char *wwwpath) {
  char *buf = malloc(BUFSIZE);
  status_t rc = OK;
  connstatus_t cs;
  request_t req;
  int keepalive = 0;

  set_timeout(cfd);

  cs = get_req(cfd, buf);
  if (cs != RecvOK)
    goto done;

  rc = parse_req(buf, &req);
  keepalive = req.keepalive;
  send_reply(cfd, rc, &req, wwwpath);

  while (keepalive) {
    memset(buf, 0, BUFSIZE);

    cs = get_req(cfd, buf);
    if (cs != RecvOK)
      goto done;

    rc = parse_req(buf, &req);
    send_reply(cfd, rc, &req, wwwpath);
  }

done:
  close(cfd);
  free(buf);
}



/*
 * handle_thread - wrapper function for pthreads calling handle_req
 * @param arg - a reqthread_t object with the client fd and www path
 * @returns NULL
 */
void *handle_thread(void *arg) {
  reqthread_t *rt = (reqthread_t *)arg;

  handle_req(rt->cfd, rt->wwwpath);
  close(rt->cfd);
  return NULL;
}



int main(int argc, char **argv) {
  char *portstr = strdup(PORTNO);
  char *wwwpath = strdup("htdocs");
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_in them;
  socklen_t themlen;
  char ch, s[INET_ADDRSTRLEN];
  int ret, sfd = -1, afd = -1;
  int threadcount = 0;
  pthread_t tids[MAXTHREADS];
  reqthread_t rt[MAXTHREADS];

  while ((ch = getopt(argc, argv, "d:p:")) != -1) {
    switch (ch) {
    case 'd':
      wwwpath = strdup(optarg);
      break;
    case 'p':
      portstr = strdup(optarg);
      break;
    default:
      fprintf(stderr, "usage: httpserver [-p port] [-d directory]\n");
    }
  }

#ifdef USE_THREADS
  printf("httpserver starting (threaded)\n");
#else
  printf("httpserver starting (non-threaded)\n");
#endif

  init_mimetypes();

  /* implement me */

  /* example code to have a single-thread or multi-thread server */

#ifdef USE_THREADS
  rt[threadcount].cfd     = afd;       // client file descriptor
  rt[threadcount].wwwpath = wwwpath;   // hdocs path
  pthread_create(&tids[threadcount], NULL, handle_thread, &rt[threadcount]); // create thread
  threadcount++;                       // increment thread cont
  assert(threadcount < MAXTHREADS);    // die if too many threads
#else
  handle_req(afd, wwwpath);            // single-thread -- handle request
#endif /* USE_THREADS */


  free(wwwpath);
  free(portstr);
  return 0;
}
