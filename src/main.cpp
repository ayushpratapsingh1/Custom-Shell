#include <iostream>

int main() {
  // Flush after every std::cout / std:cerr
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  while(true) {
  // Uncomment this block to pass the first stage
  std::cout << "$ ";
  std::string input;
  std::getline(std::cin, input);
  if(input.substr(0,4)=="echo") {
    if(input.length()>5 && input[4]==' ')
	std::cout<<input.substr(5)<<"\n";
    else
	std::cout<<"\n";
    continue;
  }
  if(input.substr(0,4)=="type") {
    if(input.length()>5 && input[4]==' ')
	if(input.substr(5)=="echo" || input.substr(5)=="type" || input.substr(5)=="exit")
	   std::cout<<input.substr(5)<<" is a shell builtin\n";
    else
	std::cout<<input.substr(5) <<": not found\n";
    continue;
  }
  if(input=="exit 0") exit(0);
  std::cout<<input<<": command not found\n";
  }
}
