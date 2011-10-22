#include <curl/curl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct handle_t {
	char *buffer;
	size_t buffer_size;
	enum { ERR_OK, ERR_OOM, ERR_OTHER } error;
	CURL *curl;
} handle_t;

static size_t
smlcurl_collect_data (void *contents, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	handle_t *hdl = (handle_t *)userp;
	char *data;

	/* There has been an error, so skip the data */
	if (hdl->error != ERR_OK)
		return realsize;

	data = realloc(hdl->buffer, hdl->buffer_size + realsize + 1);
	/* OOM */
	if (!data) {
		hdl->error = ERR_OOM;
		/* Give up */
		return realsize;
	}

	hdl->buffer = data;

	/* Copy the data */
	memcpy(&(hdl->buffer[hdl->buffer_size]), contents, realsize);
	hdl->buffer_size += realsize;
	hdl->buffer[hdl->buffer_size] = 0;

	return realsize;
}

struct handle_t *smlcurl_easy_init(void)
{
	handle_t *hdl = malloc(sizeof(handle_t));
	hdl->buffer = NULL;
	hdl->buffer_size = 0;
	hdl->error = ERR_OK;
	hdl->curl = curl_easy_init();
	return hdl;
}

int smlcurl_easy_set_verbose(handle_t *hdl, bool verbose)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_VERBOSE, verbose ? 1L : 0L);
}

int smlcurl_easy_set_header(handle_t *hdl, bool header)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_HEADER, header ? 1L : 0L);
}

int smlcurl_easy_set_progress(handle_t *hdl, bool progress)
{
	return curl_easy_setopt(hdl->curl, 
		CURLOPT_NOPROGRESS, progress ? 0L : 1L);
}

int smlcurl_easy_set_signal(handle_t *hdl, bool signal)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_NOSIGNAL, signal ? 0L : 1L);
}

int smlcurl_easy_set_url(handle_t *hdl, const char *url)
{
	int r= curl_easy_setopt(hdl->curl, CURLOPT_URL, url);
	return r;
}

int smlcurl_easy_set_proxy(handle_t *hdl, const char *proxy)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_PROXY, proxy);
}

int smlcurl_easy_set_useragent(handle_t *hdl, const char *useragent)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_USERAGENT, useragent);
}

int smlcurl_easy_perform_as_string(handle_t *hdl)
{
	  curl_easy_setopt(hdl->curl, 
	  	CURLOPT_WRITEFUNCTION, smlcurl_collect_data);
	  curl_easy_setopt(hdl->curl, 
	  	CURLOPT_WRITEDATA, (void *)hdl);
	  curl_easy_perform(hdl->curl);
	return 0;
}

const char *smlcurl_easy_get_contents(handle_t *hdl)
{
	return hdl->buffer ? hdl->buffer : "";
}

const char *smlcurl_easy_get_effective_url(handle_t *hdl)
{
	char *ret;
	int err=curl_easy_getinfo(hdl->curl, CURLINFO_EFFECTIVE_URL, &ret);

	return ret;
}

int smlcurl_easy_get_response_code(handle_t *hdl)
{
	long ret;
	int err=curl_easy_getinfo(hdl->curl, CURLINFO_RESPONSE_CODE, &ret);

	return ret;
}

int smlcurl_easy_get_connect_code(handle_t *hdl)
{
	long ret;
	int err=curl_easy_getinfo(hdl->curl, CURLINFO_HTTP_CONNECTCODE, &ret);

	return ret;
}

double smlcurl_easy_get_total_time(handle_t *hdl)
{
	double ret;
	int err=curl_easy_getinfo(hdl->curl, CURLINFO_TOTAL_TIME, &ret);

	return ret;
	
}

const char * smlcurl_easy_get_content_type(handle_t *hdl)
{
	char *ret;
	int err=curl_easy_getinfo(hdl->curl, CURLINFO_CONTENT_TYPE, &ret);

	return ret ? ret : "";
}


void smlcurl_easy_cleanup(handle_t *hdl)
{
	free(hdl->buffer);
	curl_easy_cleanup(hdl->curl);
}

