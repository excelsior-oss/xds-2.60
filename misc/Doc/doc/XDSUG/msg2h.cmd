/**/
do while lines() <> 0
  parse linein number message
  if (message <> '') & (substr(number,1,1) <> '%') then
  say '#define MESSAGE_'number number
end