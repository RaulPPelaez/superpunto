#include "RShaderProgram.h"
#include "RFile.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
namespace superpunto {
namespace detail {

GLint safeGetUniformLocation(GLuint program, const GLchar *name) {
  GLint loc = glGetUniformLocation(program, name);
  if (loc == -1) {
    System::log<System::ERROR>(
        "[ShaderProgram] Uniform %s not found in program %d", name, program);
  }
  return loc;
}
} // namespace detail

RShader::RShader() {}
RShader::~RShader() { glDeleteShader(sid); }

bool RShader::charload(const GLchar *src, GLenum type) {
  if (!src)
    return false;
  tp = type;
  sid = glCreateShader(type);
  glShaderSource(sid, 1, &src, NULL);
  glCompileShader(sid);
  int iCompilationStatus;
  glGetShaderiv(sid, GL_COMPILE_STATUS, &iCompilationStatus);
  if (iCompilationStatus == GL_FALSE) {
    std::cerr << "Could not compile shader:" << std::endl;
    char buffer[512];
    glGetShaderInfoLog(sid, 512, NULL, buffer);
    std::cerr << buffer << std::endl;
    return false;
  }
  int bufflen;
  glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &bufflen);
  if (bufflen > 1) {
    std::vector<GLchar> log_string(bufflen + 1);
    glGetShaderInfoLog(sid, bufflen, 0, log_string.data());
  }

  return true;
}

bool RShader::load(const char *fileName, GLenum type) {
  return charload(read_file(fileName).c_str(), type);
}

RShaderProgram::RShaderProgram() { pid = glCreateProgram(); }
RShaderProgram::~RShaderProgram() { glDeleteProgram(pid); }

bool RShaderProgram::init(RShader *shader_list, uint nshaders) {
  fori(0, nshaders) glAttachShader(pid, shader_list[i].id());
  glLinkProgram(pid);
  int isLinked = 0;
  glGetProgramiv(pid, GL_LINK_STATUS, (int *)&isLinked);
  if (isLinked == GL_FALSE) {
    int bl = 0;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &bl);
    std::vector<GLchar> infoLog(bl);
    glGetProgramInfoLog(pid, bl, &bl, &infoLog[0]);
    printf("%s\n", infoLog.data());
  }
  return true;
}

void RShaderProgram::use() { glUseProgram(pid); }

void RShaderProgram::unbind() { glUseProgram(0); }

template <>
void RShaderProgram::setUniform<GLint>(const GLchar *flag, const GLint *val,
                                       int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  if (count == 1)
    glProgramUniform1i(pid, loc, *val);
  else
    glProgramUniform1iv(pid, loc, count, val);
}

template <>
void RShaderProgram::setUniform<GLuint>(const GLchar *flag, const GLuint *val,
                                        int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  if (count == 1)
    glProgramUniform1ui(pid, loc, *val);
  else
    glProgramUniform1uiv(pid, loc, count, val);
}

template <>
void RShaderProgram::setUniform<float>(const GLchar *flag, const float *val,
                                       int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  if (count == 1)
    glProgramUniform1f(pid, loc, *val);
  else
    glProgramUniform1fv(pid, loc, count, val);
}

template <>
void RShaderProgram::setUniform<glm::vec2>(const GLchar *flag,
                                           const glm::vec2 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniform2fv(pid, loc, count, glm::value_ptr(val[0]));
}
template <>
void RShaderProgram::setUniform<glm::ivec2>(const GLchar *flag,
                                            const glm::ivec2 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniform2iv(pid, loc, count, glm::value_ptr(val[0]));
}

template <>
void RShaderProgram::setUniform<glm::vec3>(const GLchar *flag,
                                           const glm::vec3 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniform3fv(pid, loc, count, glm::value_ptr(val[0]));
}

template <>
void RShaderProgram::setUniform<glm::vec4>(const GLchar *flag,
                                           const glm::vec4 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniform4fv(pid, loc, count, glm::value_ptr(val[0]));
}

template <>
void RShaderProgram::setUniform<glm::mat2>(const GLchar *flag,
                                           const glm::mat2 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniformMatrix2fv(pid, loc, count, GL_FALSE, glm::value_ptr(val[0]));
}

template <>
void RShaderProgram::setUniform<glm::mat3>(const GLchar *flag,
                                           const glm::mat3 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniformMatrix3fv(pid, loc, count, GL_FALSE, glm::value_ptr(val[0]));
}

template <>
void RShaderProgram::setUniform<glm::mat4>(const GLchar *flag,
                                           const glm::mat4 *val, int count) {
  GLint loc = detail::safeGetUniformLocation(pid, flag);
  glProgramUniformMatrix4fv(pid, loc, count, GL_FALSE, glm::value_ptr(val[0]));
}

} // namespace superpunto
