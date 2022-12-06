#include <vector>

#include "Shaders.hpp"
#include "lighting.hpp"
#include "textures.hpp"

int Shader::fixedPipeline = 0;
int Shader::brickShader;
int Shader::threeDshader;

std::vector<int> shaderStack;
int currentShader;

void InitShaders()
{
   Shader::brickShader = CreateShaderProg("model.vert", "brick.frag");
	Shader::threeDshader = CreateShaderProg("pixtex.vert", "pixtex.frag");
}

void UseShader(int shader)
{
   currentShader = shader;
   if (shader == Shader::fixedPipeline)
   {
      glUseProgram(0);
   }
   else if (shader == Shader::brickShader || shader == Shader::threeDshader)
   {
      glUseProgram(shader);
      int id;

      // Set lights
      id = glGetUniformLocation(shader,"ambientLight");
      glUniform1i(id, Globals::Lighting::ambient->GetConst());
      id = glGetUniformLocation(shader,"directedLight");
      glUniform1i(id, Globals::Lighting::directed->GetConst());

      // // Set textures (ref https://stackoverflow.com/a/25252981)
      id = glGetUniformLocation(shader, "tex0");
      glUniform1i(id, 0);
      glActiveTexture(GL_TEXTURE0);
      BindTexture("dirt");
      //---
      id = glGetUniformLocation(shader, "tex1");
      glUniform1i(id, 1);
      glActiveTexture(GL_TEXTURE1);
      BindTexture("dirtF");

      // Reset active texture to 0 otherwise fixed pipeline stuff gets weird
      glActiveTexture(GL_TEXTURE0);
   }
   else
   {
      Fatal(999, "Invalid shader %d\n", shader);
   }
}

void PushShader(int shader)
{
   shaderStack.push_back(currentShader);
   UseShader(shader);
}

void PopShader()
{
   int shader = shaderStack.back();
   shaderStack.pop_back();
   UseShader(shader);
}

/************************************
* End of my code, Ex. 25 below here
*************************************/

/*
 *  Read text file
 */
char* ReadText(const char *file)
{
   char* buffer;
   //  Open file
   FILE* f = fopen(file,"rt");
   if (!f) Fatal(999, "Cannot open text file %s\n",file);
   //  Seek to end to determine size, then rewind
   fseek(f,0,SEEK_END);
   int n = ftell(f);
   rewind(f);
   //  Allocate memory for the whole file
   buffer = (char*)malloc(n+1);
   if (!buffer) Fatal(999, "Cannot allocate %d bytes for text file %s\n",n+1,file);
   //  Snarf the file
   if (fread(buffer,n,1,f)!=1) Fatal(999, "Cannot read %d bytes for text file %s\n",n,file);
   buffer[n] = 0;
   //  Close and return
   fclose(f);
   return buffer;
}

/*
 *  Print Shader Log
 */
void PrintShaderLog(int obj, std::string file)
{
   int len=0;
   glGetShaderiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal(999, "Cannot allocate %d bytes of text for shader log\n",len);
      glGetShaderInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s:\n%s\n",file.c_str(),buffer);
      free(buffer);
   }
   glGetShaderiv(obj,GL_COMPILE_STATUS,&len);
   if (!len) Fatal(999, "Error compiling %s\n",file);
}

/*
 *  Print Program Log
 */
void PrintProgramLog(int obj)
{
   int len=0;
   glGetProgramiv(obj,GL_INFO_LOG_LENGTH,&len);
   if (len>1)
   {
      int n=0;
      char* buffer = (char *)malloc(len);
      if (!buffer) Fatal(999, "Cannot allocate %d bytes of text for program log\n",len);
      glGetProgramInfoLog(obj,len,&n,buffer);
      fprintf(stderr,"%s\n",buffer);
   }
   glGetProgramiv(obj,GL_LINK_STATUS,&len);
   if (!len) Fatal(999, "Error linking program\n");
}

/*
 *  Create Shader
 */
int CreateShader(GLenum type, std::string file)
{
   //  Create the shader
   int shader = glCreateShader(type);
   //  Load source code from file
   char* source = ReadText(file.c_str());
   glShaderSource(shader,1,(const char**)&source,NULL);
   free(source);
   //  Compile the shader
   fprintf(stderr,"Compile %s\n",file.c_str());
   glCompileShader(shader);
   //  Check for errors
   PrintShaderLog(shader,file);
   //  Return name
   return shader;
}

/*
 *  Create Shader Program
 */
int CreateShaderProg(std::string VertFile, std::string FragFile)
{
   //  Create program
   int prog = glCreateProgram();
   //  Create and compile vertex shader
   int vert = CreateShader(GL_VERTEX_SHADER, ("Shaders/" + VertFile).c_str());
   //  Create and compile fragment shader
   int frag = CreateShader(GL_FRAGMENT_SHADER, ("Shaders/" + FragFile).c_str());
   //  Attach vertex shader
   glAttachShader(prog,vert);
   //  Attach fragment shader
   glAttachShader(prog,frag);
   //  Link program
   glLinkProgram(prog);
   //  Check for errors
   PrintProgramLog(prog);
   //  Return name
   return prog;
}
