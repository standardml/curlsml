signature CURL =
   sig
      val fetchURL : string -> string
   end

structure Curl :> CURL =
   struct
      fun fetchURL url
         = let 
	      val hdl = CurlFFI.curl_easy_init ();
	   in
	    CurlFFI.curl_easy_set_verbose hdl false;
	    CurlFFI.curl_easy_set_progress hdl false;
	    CurlFFI.curl_easy_set_url hdl url;
	    CurlFFI.curl_easy_set_useragent hdl "smlcurl";
	    let 
	       val contents = CurlFFI.curl_easy_get_contents hdl
	    in
	      CurlFFI.curl_easy_cleanup hdl;
	      contents
	    end
	   end
      end
