# ── Link smoke test: small application using ONLY public headers + archive ──
add_executable(jflx_link_smoke link_smoke.cpp)
target_link_libraries(jflx_link_smoke PRIVATE JFLXLIBS)
add_test(NAME link_smoke COMMAND jflx_link_smoke)

# ── Each public header must compile and link on its own (self-containment) ──
foreach(header audio text texture)
    add_executable(jflx_header_${header} header_${header}.cpp)
    target_link_libraries(jflx_header_${header} PRIVATE JFLXLIBS)
    add_test(NAME header_${header} COMMAND jflx_header_${header})
endforeach()

# ── The umbrella header must expose every sub-library and be includable from
#    more than one translation unit (umbrella_second.cpp is the second TU) ──
add_executable(jflx_header_umbrella header_umbrella.cpp umbrella_second.cpp)
target_link_libraries(jflx_header_umbrella PRIVATE JFLXLIBS)
add_test(NAME header_umbrella COMMAND jflx_header_umbrella)
