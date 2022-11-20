add_rules('mode.debug')

package('dylib')
    set_kind("library", {headeronly = true})
    set_homepage("https://github.com/martin-olivier/dylib")
    set_description("C++ cross-platform wrapper around dynamic loading of shared libraries (dll, so, dylib)")
    set_license("MIT")
    add_urls("https://github.com/martin-olivier/dylib/archive/refs/tags/$(version).tar.gz")
    add_versions("v2.1.0", 'f48b68549b93aeb794d003793329f82e82b193dda26797c23efb3a5e83d4e33b')

    on_install("windows", "linux", "mingw", "macosx", function (package)
        os.cp("include/dylib.hpp", package:installdir("include"))
    end)
package_end()
add_requires('nlohmann_json', 'dylib')

target("HTMLWriter")
    add_files('modules/HTMLWriter/src/*.cpp')
    add_includedirs('modules/HTMLWriter/include/', 'include/common/')
    add_packages('nlohmann_json', 'dylib')
    set_symbols("debug")
    set_kind('shared')
    set_languages('cxx20')
    set_warnings('everything')
    set_targetdir("$(buildir)/$(plat)/$(arch)/$(mode)/modules")
    if (is_plat("linux")) then
        add_syslinks("dl")
    end


target('PLDCreator')
    set_symbols("debug")
    set_kind('binary')
    set_languages('cxx20')
    add_files('src/core/*.cpp', 'src/common/*.cpp')
    add_includedirs('include/core/', 'include/common/')
    add_packages('nlohmann_json', 'dylib')
    set_warnings('everything')
    if (is_plat("linux")) then
        add_syslinks("dl")
    end
    after_build(function (target)
        print("Copying assets to build directory...")
        local binaryPath = "$(buildir)/$(plat)/$(arch)/$(mode)"
        os.cp("config.json", binaryPath .. "/config.json")
        os.cp("pld.json", binaryPath .. "/pld.json")
        os.mkdir(binaryPath .. "/assets")
        os.cp("assets/**",  binaryPath .. "/assets")
        print("Copying assets to build directory... Done !")
    end)