Invoke-Expression -Command "autotester.exe iter2_1_source_calls.txt iter2_1_queries_calls.txt xmls/iter2_1_out.xml"

Invoke-Expression -Command "autotester.exe iter2_2_source_complex_calls_graph.txt iter2_2_queries_complex_calls_graph.txt xmls/iter2_2_1_out.xml"
Invoke-Expression -Command "autotester.exe iter2_2_source_complex_calls_graph.txt iter2_2_queries_alternate_output_attributes.txt xmls/iter2_2_2_out.xml"

Invoke-Expression -Command "autotester.exe iter2_3_source_patternAssign.txt iter2_3_queries_patternAssign.txt xmls/iter2_3_out.xml"

Invoke-Expression -Command "autotester.exe iter2_4_source_complex_callst_graph.txt iter2_4_queries_complex_callst_graph.txt xmls/iter2_4_out.xml"

Invoke-Expression -Command "autotester.exe iter2_5_source_BOOLEAN.txt iter2_5_queries_BOOLEAN_.txt xmls/iter2_5_out.xml"

Invoke-Expression -Command "autotester.exe iter2_6_source_with_.txt iter2_6_queries_with.txt xmls/iter2_6_out.xml"

Invoke-Expression -Command "autotester.exe iter2_7_source_Next.txt iter2_7_queries_Next_.txt xmls/iter2_7_out.xml"

Invoke-Expression -Command "autotester.exe iter2_8_source_NextT.txt iter2_8_queries_NextT.txt xmls/iter2_8_out.xml"


