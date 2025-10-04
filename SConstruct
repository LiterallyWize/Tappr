env = SConscript("godot-cpp/SConstruct")
env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp")

if env["platform"] == "windows":
	env.Append(LIBS=["user32"])
	env.Append(CPPDEFINES=["WIN32_LEAN_AND_MEAN", "NOCOMM", "STRICT"])

	library = env.SharedLibrary(
		"project/bin/tappr/tappr{}{}".format(env["suffix"], env["SHLIBSUFFIX"]),
		source=sources,
	)
	Default(library)

elif env["platform"] == "macos":
	print("! macOS currently unsupported")
	Exit(1)

elif env["platform"] == "linux":
	print("! Linux currently unsupported")
	Exit(1)

else:
	print("! Unsupported platform")
	Exit(1)
