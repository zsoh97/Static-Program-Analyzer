Invoke-Expression -Command "autotester.exe iter2_cyclic_call_source_1.txt empty_queries.txt out.xml"
Invoke-Expression -Command "autotester.exe iter2_cyclic_call_source_2.txt empty_queries.txt out.xml"

Invoke-Expression -Command "autotester.exe iter2_non_existing_proc_source_1.txt empty_queries.txt out.xml"
Invoke-Expression -Command "autotester.exe iter2_non_existing_proc_source_2.txt empty_queries.txt out.xml"

Invoke-Expression -Command "autotester.exe iter2_recursive_call_1.txt empty_queries.txt out.xml"
Invoke-Expression -Command "autotester.exe iter2_recursive_call_2.txt empty_queries.txt out.xml"

Invoke-Expression -Command "autotester.exe iter2_same_name_source_1.txt empty_queries.txt out.xml"
Invoke-Expression -Command "autotester.exe iter2_same_name_source_2.txt empty_queries.txt out.xml"
