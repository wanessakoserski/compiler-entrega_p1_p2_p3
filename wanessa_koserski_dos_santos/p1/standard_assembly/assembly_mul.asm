.values
SET x, 3
SET y, 3
SET result, 0
SET counter, 0
SET mone, -1

.steps
LDA y
STA counter
LBL l_loop
LDA counter
JZ l_end
LDA result
ADD x
STA result
LDA counter
ADD mone
STA counter
JMP l_loop
LBL l_end 
HLT