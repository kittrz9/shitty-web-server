# shitty web server
a web server written in C to host static files stored in a tar file<br>
<hr>

## building
you should just be able to run `./build.sh` in this directory and it should compile everything<br>
you should also run `./pack.sh` to pack everything in the `html/` directory into a tar file<br>

## usage
just run the executable in the `build/` directory as root with the tar file you want to serve as the first argument.<br><br>
it has to be run as root to have access to port 80, if you change the port to something above or equal to 1024 it should be able to run as a normal user.<br>
<hr>

## why a tar file?
partly because having the web server confined to a single real file instead of the whole file system seemed like it would reduce any risk of it serving some file from outside it's intended directory a lot (though security really isn't this projects main goal lmao).<br><br>
partly because I kinda like the idea of everything it hosts being contained in a single file that could be compressed with something like bz2.<br><br>
partly because I already had code to parse [USTAR](https://wiki.osdev.org/USTAR) formatted files from [my os dev project](https://github.com/kittrz9/os-dev-thing).<br><br>
mostly because I thought it was an interesting idea.<br><br>

## is it good enough to use?
probably not lmao<br><br>
idk anything about designing something with actual security in mind. there's probably some RCE vulnerability in this that I'm too stupid to find. just use something like nginx.<br>
