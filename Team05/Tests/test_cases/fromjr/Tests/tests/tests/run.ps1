#Invoke-Expression -Command "autotester.exe 1-level_source.txt 1-level_queries.txt 1-level.xml"
#Invoke-Expression -Command "autotester.exe 3-level_source.txt 3-level_queries.txt 3-level.xml"
#Invoke-Expression -Command "autotester.exe 3-proc_source.txt 3-proc_queries.txt 3-proc.xml"
#Invoke-Expression -Command "autotester.exe assign_source.txt assign_queries.txt assign.xml"
#Invoke-Expression -Command "autotester.exe ez_source.txt ez_queries.txt ez.xml"
#Invoke-Expression -Command "autotester.exe flat_source.txt flat_queries.txt flat.xml"
#Invoke-Expression -Command "autotester.exe multi_proc_source.txt multi_proc_queries.txt multip.xml"
Invoke-Expression -Command "autotester.exe nested_containers_source.txt nested_containers_queries.txt nestedcon.xml"
Invoke-Expression -Command "autotester.exe nested_containers_source.txt nested_containers_affects_queries.txt nestedconA.xml"
Invoke-Expression -Command "autotester.exe weird_source.txt weird_queries.txt weird.xml"