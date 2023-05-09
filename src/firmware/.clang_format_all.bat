for /r src %%t in (*.cpp *.hpp) do clang-format -i -style=file "%%t"
for /r lib %%t in (*.cpp *.hpp) do clang-format -i -style=file "%%t"