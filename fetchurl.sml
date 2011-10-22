
val _ = map (fn url => print (Curl.fetchURL url)) (CommandLine.arguments ())
