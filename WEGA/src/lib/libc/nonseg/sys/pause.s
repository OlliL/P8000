! @[$] pause.s	2.1  09/12/83 11:42:42 - 87wega3.2 !

pause module

  global
    _pause procedure
      entry
        sc	#29
        ret
    end _pause
end pause
