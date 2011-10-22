#include <curl/curl.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

typedef struct handle_t {
	bool inuse;
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
	hdl->inuse = true;
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
	return curl_easy_setopt(hdl->curl, CURLOPT_URL, url);
}

int smlcurl_easy_set_proxy(handle_t *hdl, const char *proxy)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_PROXY, proxy);
}

int smlcurl_easy_set_useragent(handle_t *hdl, const char *useragent)
{
	return curl_easy_setopt(hdl->curl, CURLOPT_USERAGENT, useragent);
}

void smlcurl_easy_perform_as_string(handle_t *hdl)
{
	  curl_easy_setopt(hdl, CURLOPT_WRITEFUNCTION, smlcurl_collect_data);
	  curl_easy_setopt(hdl, CURLOPT_WRITEDATA, (void *)&hdl);
	  curl_easy_perform(hdl->curl);
}

const char *smlcurl_get_contents(handle_t *hdl)
{
	return hdl->buffer ? "" : hdl->buffer;
}

void smlcurl_easy_cleanup(handle_t *hdl)
{
	curl_easy_cleanup(hdl);
}
