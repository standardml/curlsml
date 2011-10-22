signature CURLFFI = 
   sig
      type hdl

      val curl_easy_init : unit -> hdl
      val curl_easy_set_verbose : hdl -> bool -> unit
      val curl_easy_set_header : hdl -> bool -> unit
      val curl_easy_set_progress : hdl -> bool -> unit
      val curl_easy_set_signal : hdl -> bool -> unit
      val curl_easy_set_url : hdl -> string -> unit
      val curl_easy_set_proxy : hdl -> string -> unit
      val curl_easy_set_useragent : hdl -> string -> unit
      val curl_easy_perform_as_string : hdl -> unit
      val curl_easy_get_contents : hdl -> string
      val curl_easy_get_effective_url : hdl -> string
      val curl_easy_get_response_code : hdl -> int
      val curl_easy_get_connect_code : hdl -> int
      val curl_easy_get_total_time : hdl -> real
      val curl_easy_get_content_type : hdl -> string
      val curl_easy_cleanup : hdl -> unit
    end

structure CurlFFI :> CURLFFI =
   struct
      type hdl = MLton.Pointer.t
      type cstr = CString.p

(* Raw API, We'll build the public API out of this *)
      val curl_easy_init_raw =
        _import "smlcurl_easy_init" : unit-> hdl;

      (* Boolean options *)
      val curl_easy_set_verbose_raw =
        _import "smlcurl_easy_set_verbose" : (hdl * bool) -> int;

      val curl_easy_set_header_raw =
        _import "smlcurl_easy_set_header" : (hdl * bool) -> int;

      val curl_easy_set_progress_raw =
        _import "smlcurl_easy_set_progress" : (hdl * bool) -> int;

      val curl_easy_set_signal_raw =
        _import "smlcurl_easy_set_signal" : (hdl * bool) -> int;

      (* String options *)
      val curl_easy_set_url_raw =
        _import "smlcurl_easy_set_url" : (hdl * cstr) -> int;

      val curl_easy_set_proxy_raw =
        _import "smlcurl_easy_set_proxy" : (hdl * cstr) -> int;

      val curl_easy_set_useragent_raw =
      	_import "smlcurl_easy_set_useragent" : (hdl * cstr) -> int;

      (* Perform *)
      val curl_easy_perform_as_string_raw =
        _import "smlcurl_easy_perform_as_string" : hdl -> int;

      val curl_easy_get_contents_raw =
        _import "smlcurl_easy_get_contents" : hdl -> cstr;

      val curl_easy_cleanup_raw =
        _import "smlcurl_easy_cleanup" : hdl -> unit;

      val curl_easy_get_effective_url_raw =
        _import "smlcurl_easy_get_effective_url" : hdl -> cstr;

      val curl_easy_get_response_code_raw =
        _import "curl_easy_get_response_code" : hdl -> int;

      val curl_easy_get_connect_code_raw =
        _import "curl_easy_get_connect_code" : hdl -> int;

      val curl_easy_get_total_time_raw =
        _import "curl_easy_get_total_time" : hdl -> real;

      val curl_easy_get_content_type_raw =
        _import "curl_easy_get_content_type" : hdl -> cstr;

(* Helper functions *)
      fun fail_on_error 0 = ()
        | fail_on_error x = raise Fail ("Unexpected curl error #" ^ (Int.toString x))
	;

      fun cstr_to_string cstr = CString.toString (CString.fromPointer cstr)

      fun call_with_cstr f hdl str 
        = CString.app (fn cstr => f (hdl, cstr)) (CString.fromString str)
        

(* The public API *)
      val curl_easy_init = curl_easy_init_raw;

      fun curl_easy_set_verbose hdl flag 
        = fail_on_error (curl_easy_set_verbose_raw (hdl, flag))
      fun curl_easy_set_header hdl flag 
        = fail_on_error (curl_easy_set_header_raw (hdl, flag))
      fun curl_easy_set_progress hdl flag 
        = fail_on_error (curl_easy_set_progress_raw (hdl, flag))
      fun curl_easy_set_signal hdl flag 
        = fail_on_error (curl_easy_set_progress_raw (hdl, flag))

      fun curl_easy_set_url hdl value 
        = fail_on_error (call_with_cstr curl_easy_set_url_raw hdl value)
      fun curl_easy_set_proxy hdl value 
        = fail_on_error (call_with_cstr curl_easy_set_proxy_raw hdl value)
      fun curl_easy_set_useragent hdl value 
        = fail_on_error (call_with_cstr curl_easy_set_useragent_raw hdl value)

      fun curl_easy_perform_as_string hdl 
        = fail_on_error (curl_easy_perform_as_string_raw (hdl))

      fun curl_easy_get_contents hdl
        = cstr_to_string (curl_easy_get_contents_raw hdl)

      fun curl_easy_get_effective_url hdl
        = cstr_to_string (curl_easy_get_effective_url_raw hdl)

      val curl_easy_get_response_code = curl_easy_get_response_code_raw; 

      val curl_easy_get_connect_code = curl_easy_get_connect_code_raw;

      val curl_easy_get_total_time = curl_easy_get_total_time_raw;

      fun curl_easy_get_content_type hdl
        = cstr_to_string (curl_easy_get_content_type_raw hdl)

      fun curl_easy_cleanup hdl = curl_easy_cleanup_raw hdl

  end
