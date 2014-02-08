
#~! using: avisynth26a5.dll
#~! test: avisynth.dll

# type    : dll  => test from main dll to other dll
#           func => test across two functions, run in main dll {NOT YET IMPLEMENTED}
#           img  => compare result to stored image {NOT YET IMPLEMENTED}

# test    : output => test whether output is correct
#           perf   => test performance (can't use with type=img) {NOT YET IMPLEMENTED}

# cmd     : an avisynth command to be Eval'd to get result/test performance. Must return clip.

# frame   : all [default] => run all frame sequentially
#           => specify frame number or range, delimited by comma, e.g. 1-100,200,300-400

#~! case "Upscaling #1": type=dll test=output frame=all cmd=scaling_test0()
#~! case "Upscaling #2": type=dll test=output frame=all cmd=scaling_test1()
#~! case "Downscaling #1": type=dll test=output frame=all cmd=scaling_test2()
#~! case "Downscaling #2": type=dll test=output frame=all cmd=scaling_test3()

function scaling_test0() {
	blankclip(width=1280, height=720, pixel_type="YV12",color=$000000)

	addgrainc(1000,1000,seed=1)
	spline64resize(1920,1080)
}
function scaling_test() {
	blankclip(width=1280, height=720, pixel_type="YV12",color=$000000)

	addgrainc(1000,1000,seed=1)
	converttorgb32
	spline64resize(1920,1080)
	converttoyv16
}

function scaling_test2() {
	blankclip(width=1920, height=1080, pixel_type="YV12",color=$000000)

	addgrainc(1000,1000,seed=1)
	spline64resize(1280,720)
}

function scaling_test3() {
	blankclip(width=1920, height=1080, pixel_type="YV12",color=$000000)

	addgrainc(1000,1000,seed=1)
	converttorgb32
	spline64resize(1280,720)
	converttoyv16
}
scaling_test()
