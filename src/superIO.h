/*Raul P. Pelaez 2018. Read lines of a file into numbers in an array and viceversa
  See https://raulppelaez.github.io/c++/conversion/string/io/2018/01/17/fastest-cpp-string-to-double.html

  If USE_BOST is defined, it will use boost:spirit and boost:karma to parse strings into numbers and viceversa, this is ~20% faster than std.
*/
#ifndef SUPER_READ_H
#define SUPER_READ_H

#include<vector>
#include<cstdlib>
#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include <string.h>

#ifdef USE_BOOST
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_int.hpp>
#include<boost/spirit/include/karma.hpp>
#endif

namespace superIO{
  
#ifdef USE_BOOST

  template<class T>
  void qiparse(const char * start, const char *end, T& value){
    boost::spirit::qi::parse(start, end , boost::spirit::qi::double_, value);
  }
  template<>
  void qiparse<int>(const char * start, const char *end, int& value){
    boost::spirit::qi::parse(start, end , boost::spirit::qi::int_, value);
  }


#endif



  template<class T>
  inline int string2numbersBoost(const char *line, int nCharacters, int ncols, T *readedValues){
    int endCharacter = 0;
    int firstCharacter = 0; //First character of a number
    for(int i=0; i<ncols; i++){

      //First char of next number is first non space char.
      while(std::isspace(line[firstCharacter])
	    and firstCharacter<nCharacters){
	firstCharacter++;
      }
      //Last char is first space char
      endCharacter = firstCharacter;
      while(!std::isspace(line[endCharacter])
	    and endCharacter<nCharacters){
	endCharacter++;
      }
      T value;
      qiparse(line+firstCharacter, line+endCharacter, value);
      readedValues[i] = value;
    
      endCharacter++;
      firstCharacter = endCharacter;
    
    }
    return endCharacter-1;
  }

  template<class T>
  inline int string2numbersSTD(const char *line, int ncols, T *readedValues){  
    char *l2;
    const char *l1 = line;

    for(int i=0; i<ncols; i++){
      readedValues[i] = strtod(l1, &l2); l1=l2;
    }

    return l1-line;
 
  }

  template<>
  inline int string2numbersSTD<int>(const char *line, int ncols, int *readedValues){  
    char *l2;
    const char *l1 = line;

    for(int i=0; i<ncols; i++){
      readedValues[i] = strtol(l1, &l2, 10); l1=l2;
    }

    return l1-line;
 
  }

  template<class T>
  inline int string2numbers(const char *line, int nCharacters, int ncols, T *readedValues){
    if(nCharacters <= 0) return -1;
#ifdef USE_BOOST
    return string2numbersBoost(line, nCharacters, ncols, readedValues);
#else
    return string2numbersSTD(line, ncols, readedValues);
#endif
 
  }



#ifdef USE_BOOST
  template <typename T>
  inline bool number2stringBoost(std::string &str, T const& value)
  {
    std::back_insert_iterator<std::string> sink(str); 
    return boost::spirit::karma::generate(sink, value); 
  }
#endif
  
  template <typename T>
  inline bool number2stringSTD(std::string &str, T const& value){
    str += std::to_string(value);
    return true;
  }

  template<class T>
  inline bool number2string(std::string &str, T const& value){

    #ifdef USE_BOST
    return number2stringBOOST(str, value);
    #else
    return number2stringSTD(str, value);
    #endif
  }


  class superFile{
    int READBUFSIZE=1<<22;
    int WRITEBUFSIZE=1<<22;
    std::vector<char> buf;
    char *current = nullptr, *end = nullptr;

    FILE *in = nullptr;
    bool closeFILEAtDestruction = false;
    char *endBuffer;
    bool lastChunk = false;
    int fileDescriptor = -1;


    std::vector<char> writeBuf;
    int currentWriteIndex = 0;

    bool peekedLine = false;
    char* lastPickedLine;
    int lastPickedLineSize;
  public:
    superFile(std::string fileName):superFile(fopen(fileName.c_str(), "r")){closeFILEAtDestruction =true;}
    superFile():superFile(stdin){}
    superFile(FILE *in):in(in){
      if(!in){
	//std::cerr<<"ERROR: INVALID FILE!!"<<std::endl;
	//exit(1);
	return;
      }
      fileDescriptor = fileno(in);
      if(fileDescriptor<0){
	// std::cerr<<"ERROR: CANNOT OPEN FILE!!"<<std::endl;
	// exit(1);
	fclose(in);
	in = nullptr;
	return;
      }
      buf.resize(READBUFSIZE);
      current = buf.data();
      int readedChars = read(fileDescriptor, buf.data(), READBUFSIZE*sizeof(char));
      endBuffer = buf.data()+readedChars;
      if(readedChars<0){
	// std::cerr<<"ERROR: CANNOT READ FROM INPUT!!"<<std::endl;
	// exit(1);
	fclose(in);
	in = nullptr;
	return;
      }
      if(feof(in)) lastChunk = true;

      writeBuf.resize(WRITEBUFSIZE);
      currentWriteIndex=1;
      
    }
    ~superFile(){
      if(closeFILEAtDestruction)
	if(in!=stdin && in != nullptr) fclose(in);
      if(currentWriteIndex>0)
	(void) ::write(1, writeBuf.data(), currentWriteIndex);
    }

    bool good(){
      if(!lastChunk and !in) return false;
      return true;
    }
    bool eof(){
      return lastChunk and current>=endBuffer;
    }
    //Read and return next line, but dont count it towards the next call to getNextLine.
    inline int peekLine(char *& line, char separator='\n'){
      if(!peekedLine){
	int nc = getNextLine(line, separator);
	lastPickedLine = line;
	lastPickedLineSize = nc;
      }
      this->peekedLine = true;
      line = lastPickedLine;
      return lastPickedLineSize;
    }

    inline int getNextLine(char *& line, char separator='\n'){
      //This function is equivalent to this (except for the peek mechanism)
      // static size_t  linesize2 = 0;
      // return getline(&line, &linesize2, in);
      
      //No more lines!
      if(eof()){
	line = nullptr;
	return -1;
      }
      //If the last line was peeked return it and do nothing.
      if(peekedLine){
	peekedLine = false;
	line = lastPickedLine;
	return lastPickedLineSize;
      }
      //If the current buffer has been processed, flag to read again.
      if(current>=endBuffer){
	end = nullptr;
      }
      else{ //If there is still buffer to be processed
	//Seach for a newline starting at current
	constexpr int memchrSize = 500; //memchr does not like big sizes...
	end = (char*)memchr(current, separator, memchrSize);
	//If the end of the buffer was reached before finding a newline flag to read more
	//If the newline was not found after 500 characters, try the next 500 until the end of the buffer is reached.       
	if(!end){
	  int counter = 1;
	  while(!end && (current+counter*memchrSize)<=endBuffer){
	    end = (char*)memchr(current+counter*memchrSize, separator, memchrSize);
	    counter++;
	  }
	}
      }

      //If the buffer has been processed read the next chunk.
      if(!end){
	int rnc = readNextChunk();
	//End of file
	if(rnc == -1){
	  line = nullptr;
	  return -1;
	}
	//Need to keep reading to finish line
	else if(rnc == -2) return getNextLine(line, separator);
      }

      //line points to first character of line
      line = current;
      //end points to the newline character of this line
      int linesize = end-current+1;
      //Move pointer to next line
      current = end+1;
      //Replace newline by string end
      *end ='\0';
      return linesize;
    }

    inline int readNextChunk(){
      //If it is the last chunk ending return end of file code
      if(lastChunk){
	return -1;
      }
      //If the current buffer does not end in a newline,
      // store the contents of the  current line in the begining of the buffer
      int currentReadedLineSize = int(endBuffer-current);	
      
      //I need the buffer size to be at least the size of the biggest line. Quite hackable so be careful...
      if(currentReadedLineSize>(READBUFSIZE-100)){
	READBUFSIZE += 100;
	buf.resize(READBUFSIZE);
	//TODO: This should not be a problem I guess.
	if(READBUFSIZE>1<<27){
	  std::cerr<<"ERROR: I CANNOT HANDLE LINES THAT LONG!!"<<std::endl;
	  exit(1);
	}
      }

      int toread = (READBUFSIZE-currentReadedLineSize);
      for(int i = 0; i<currentReadedLineSize; i++) buf[i] = current[i];
      
      //Read the next chuck
      int readedChars = 0;
      readedChars = read(fileDescriptor, buf.data()+currentReadedLineSize, toread*sizeof(char));

      endBuffer = buf.data()+currentReadedLineSize+readedChars;
      current = buf.data();
      //If no chunk was read the end of the file was reached
      if(readedChars == 0 || feof(in)){
	lastChunk = true;
      }	
      //<0 means error in read()
      else if(readedChars < 0){
	return -1;
      }
      //Run again with new chunk
      return -2;
    }
    
    
    inline bool iscomment(const char *line, size_t size, char comment = '#'){
      char firstC = '\0';
      for(uint i=0; i<size; i++)if(!isspace(line[i])){ firstC=line[i]; break;}
      if(firstC==comment){
	return true;
      }
      return false;
    }

    inline void write(const char * str, size_t size){
      uint i;
      for(i=0; i<size; i++){
	if((currentWriteIndex + i) == writeBuf.size()-1) break;
	writeBuf[currentWriteIndex+i] = str[i];
      }
      currentWriteIndex += i;
      if(size_t(currentWriteIndex)==writeBuf.size()-1){
	(void) ::write(1, writeBuf.data(), writeBuf.size());
	
	currentWriteIndex = 0;
	write(str+i, size-i);
      }
    }
  
    inline void setWriteBufferSize(size_t size){
      if(size_t(currentWriteIndex)>=size){
	(void) ::write(1, writeBuf.data(), currentWriteIndex);	
	currentWriteIndex = 0;
      }
      writeBuf.resize(size);
      WRITEBUFSIZE = size;
    }
    inline void flush(){
      (void) ::write(1, writeBuf.data(), currentWriteIndex);
      currentWriteIndex = 0;
}

    inline void setReadBufferSize(size_t size){
      if(size<size_t(READBUFSIZE)){
	std::cerr<<"WARNING: You can only increase the read buffer size, ignoring..."<<std::endl;
	return;
      }

      buf.resize(size);
      int readedChars = read(fileDescriptor, buf.data()+READBUFSIZE, (size-READBUFSIZE)*sizeof(char));
      if(size_t(readedChars)<(size-READBUFSIZE)) lastChunk = true;    
      endBuffer += readedChars;
      READBUFSIZE = size;
    
    }
  
  };



}
#endif
