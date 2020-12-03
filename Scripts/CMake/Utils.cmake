# Convert a string from camel case to snake case
function(nr_case_camel_to_snake str out_var)
  string(REGEX REPLACE "(.)([A-Z][a-z]+)" "\\1-\\2" ${out_var} "${str}")
  string(REGEX REPLACE "([a-z0-9])([A-Z])" "\\1-\\2" ${out_var} "${${out_var}}")
  string(TOLOWER "${${out_var}}" ${out_var})
  set(${out_var} "${${out_var}}" PARENT_SCOPE)
endfunction(nr_case_camel_to_snake)
