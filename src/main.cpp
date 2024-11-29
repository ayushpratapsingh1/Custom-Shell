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
	std::cout<<input.substr(5);
    else
	std::cout<<"\n";
    continue;
  }
  if(input=="exit 0") exit(0);
  std::cout<<input<<": command not found\n";
  }
}
