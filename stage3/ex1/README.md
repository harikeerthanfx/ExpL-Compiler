Stage 3 — Exercise 1: AST Evaluator

Run
bison -d exprtree.y
flex exprtree.l
gcc exprtree.tab.c lex.yy.c exprtree.c eval.c -o exprtree
./exprtree input.txt

Test — If-Else
begin
read(a);
read(b);
if (a < b) then
write(b);
else
write(a);
endif;
end

Input:
5
8

Expected:
8

Test — While
begin
read(a);
while (a < 5) do
write(a);
a = a + 1;
endwhile;
end

Input:
2

Expected:
2
3
4

Exercise 1 complete when both tests pass.
