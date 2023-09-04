
add_subdirectory(vr-app)

build_lib_example(
        NAME ns3ai_multibss_vec
        SOURCE_FILES
        multi-bss-vec.cc
            auto-mcs-wifi-manager.cc
            tgax-residential-propagation-loss-model.cc
        LIBRARIES_TO_LINK
            ${libai}
            ${libcore}
            ${libwifi}
            ${libinternet}
            ${libnetwork}
            ${libvr-app}
            ${libapplications}
            ${libpropagation}
            ${libenergy}
            ${libspectrum}
            ${libantenna}
            ${libbuildings}
            ${libmobility}
            ${gsl_libraries}
)

pybind11_add_module(ns3ai_multibss_vec_py multi_bss_vec_py.cc)
set_target_properties(ns3ai_multibss_vec_py PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ns3ai_multibss_vec_py PRIVATE
        ${libai}
)

# Build Python interface along with C++ lib
add_dependencies(ns3ai_multibss_vec ns3ai_multibss_vec_py)

# stru version -- for benchmarking only

build_lib_example(
        NAME ns3ai_multibss_stru
        SOURCE_FILES
        multi-bss-stru.cc
        auto-mcs-wifi-manager.cc
        tgax-residential-propagation-loss-model.cc
        LIBRARIES_TO_LINK
        ${libai}
        ${libcore}
        ${libwifi}
        ${libinternet}
        ${libnetwork}
        ${libvr-app}
        ${libapplications}
        ${libpropagation}
        ${libenergy}
        ${libspectrum}
        ${libantenna}
        ${libbuildings}
        ${libmobility}
        ${gsl_libraries}
)

pybind11_add_module(ns3ai_multibss_stru_py multi_bss_stru_py.cc)
set_target_properties(ns3ai_multibss_stru_py PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ns3ai_multibss_stru_py PRIVATE
        ${libai}
)

# Build Python interface along with C++ lib
add_dependencies(ns3ai_multibss_stru ns3ai_multibss_stru_py)

