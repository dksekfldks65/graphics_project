// Defines the entry point for the console application.
//
#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <time.h>

#include "Object.h"
#include "Camera.h"
#include "Shader.h"

#include "vec.hpp"
#include "mat.hpp"
#include "transform.hpp"
#include "SOIL.h"

void init();
void display();
void reshape(int, int);
void idle();
void keyboard(unsigned char, int, int);
void special(int, int, int);
void createGLUTMenus();
void processMenuEvents(int option);
void myMouseFunc(int button, int state, int x, int y);
void draw_a_object(float tranX,   float tranY,   float tranZ,
                   float scaleX,  float scaleY,  float scaleZ,
                   float rotateX, float rotateY, float rotateZ,
                   Object object,     float angle, char* filePath);
void draw_a_object2(float tranX,   float tranY,   float tranZ,
                  float scaleX,  float scaleY,  float scaleZ,
                  float rotateX, float rotateY, float rotateZ,
                  Object object,     float angle);

GLuint program;

GLint  loc_a_vertex;
GLint  loc_a_normal;
GLint  loc_a_texcoord;
GLint  loc_u_texid;

GLint  loc_u_pvm_matrix;
GLint  loc_u_view_matrix;
GLint  loc_u_model_matrix;
GLint  loc_u_normal_matrix;

GLint  loc_u_light_vector;

GLint  loc_u_light_ambient;
GLint  loc_u_light_diffuse;
GLint  loc_u_light_specular;

GLint  loc_u_material_ambient;
GLint  loc_u_material_diffuse;
GLint  loc_u_material_specular;
GLint  loc_u_material_shininess;

kmuvcl::math::mat4x4f   mat_PVM;

kmuvcl::math::vec4f light_vector      = kmuvcl::math::vec4f(10.0f, 10.0f, 10.0f);
kmuvcl::math::vec4f light_ambient     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_diffuse     = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);
kmuvcl::math::vec4f light_specular    = kmuvcl::math::vec4f(1.0f, 1.0f, 1.0f, 1.0f);

kmuvcl::math::mat4x4f   mat_Proj, mat_View_inv, mat_Model;

std::string scene_filename;
std::string g_filename;
std::string desk_filename;
std::string fan_filename;
std::string giraffe_filename;
std::string platform_filename;
std::string sculpture_filename;
Object      g_platform;
Object      g_model;        // object
Camera		  g_camera;											// viewer (you)
Object      g_desk;
Object      g_fan;
Object      g_giraffe;
Object      g_sculpture;
float model_scale = 5.0f;
float model_angle = 0.0f;
float fan_angle   = 0.0f;
GLuint    texid;

std::chrono::time_point<std::chrono::system_clock> prev, curr;

int main(int argc, char* argv[])
{
  if (argc > 1)
  {
    g_filename = argv[2];
  }
  else
  {
    g_filename = "./data/cube.obj";
    desk_filename = "./data/desk_with_normals.obj";
    fan_filename = "./data/fan.obj";
    giraffe_filename = "./data/Giraffe.obj";
    platform_filename = "./data/cube2.obj";
    sculpture_filename = "./data/sculpture.obj";
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowPosition(100, 100);
  glutInitWindowSize(640, 640);

  glutCreateWindow("Modeling & Navigating Your Studio");
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(special);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(myMouseFunc);
  createGLUTMenus();
  glutIdleFunc(idle);
  if (glewInit() != GLEW_OK)
  {
      std::cerr << "failed to initialize glew" << std::endl;
      return -1;
  }

  init();

  glutMainLoop();

  return 0;
}

void createGLUTMenus(){
  GLint SubMenu1 = glutCreateMenu(processMenuEvents);
  glutAddMenuEntry("3",1 );
  glutAddMenuEntry("5",2 );
  glutAddMenuEntry("7",3 );

  GLint menu = glutCreateMenu(processMenuEvents);
  glutAddSubMenu("moveMuseum",SubMenu1);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void processMenuEvents(int option){
  switch (option){

  }
}

void myMouseFunc(int button, int state, int x, int y)
{
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP) {

	}
}


void init()
{
  g_model.load_simple_obj(g_filename);
  g_desk.load_simple_obj(desk_filename);
  g_fan.load_simple_obj(fan_filename);
  g_giraffe.load_simple_obj(giraffe_filename);
  g_platform.load_simple_obj(platform_filename);
  g_sculpture.load_simple_obj(sculpture_filename);

  //g_model.print();

  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);    // for filled polygon rendering

  glEnable(GL_DEPTH_TEST);

  program = Shader::create_program("./shader/phong_vert.glsl", "./shader/phong_frag.glsl");

  loc_u_pvm_matrix         = glGetUniformLocation(program, "u_pvm_matrix");
  loc_u_view_matrix        = glGetUniformLocation(program, "u_view_matrix");
  loc_u_model_matrix       = glGetUniformLocation(program, "u_model_matrix");
  loc_u_normal_matrix      = glGetUniformLocation(program, "u_normal_matrix");

  loc_u_light_vector       = glGetUniformLocation(program, "u_light_vector");

  loc_u_light_ambient      = glGetUniformLocation(program, "u_light_ambient");
  loc_u_light_diffuse      = glGetUniformLocation(program, "u_light_diffuse");
  loc_u_light_specular     = glGetUniformLocation(program, "u_light_specular");

  loc_u_material_ambient   = glGetUniformLocation(program, "u_material_ambient");
  loc_u_material_diffuse   = glGetUniformLocation(program, "u_material_diffuse");
  loc_u_material_specular  = glGetUniformLocation(program, "u_material_specular");
  loc_u_material_shininess = glGetUniformLocation(program, "u_material_shininess");

  loc_a_vertex             = glGetAttribLocation(program, "a_vertex");
  loc_a_normal             = glGetAttribLocation(program, "a_normal");
  loc_a_texcoord 			     = glGetAttribLocation(program, "a_texcoord");
  loc_u_texid              = glGetUniformLocation(program, "u_texid");

  glGenTextures(1, &texid);

  prev = curr = std::chrono::system_clock::now();
}

void display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);

  // camera intrinsic param
  mat_Proj = kmuvcl::math::perspective(60.0f, 1.0f, 0.001f, 200.0f);

  // camera extrinsic param
  mat_View_inv= kmuvcl::math::lookAt(
  	g_camera.position()(0), g_camera.position()(1)+1.0f, g_camera.position()(2),				// eye position
  	g_camera.center_position()(0), g_camera.center_position()(1)+1.0f, g_camera.center_position()(2), // center position
  	g_camera.up_direction()(0), g_camera.up_direction()(1), g_camera.up_direction()(2)			// up direction
  	);

  draw_a_object(20.0f,   0.0f, -50.0f,
                0.5f,    0.5f, 0.5f,
                0.0f,    1.0f, 0.0f,
                g_model,  180.0f, "./data/wall2.jpg");

  draw_a_object(-5.0f,   0.0f, -25.0f,
                0.5f,    0.5f, 0.5f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/wall2.jpg");

  draw_a_object(45.0f,   0.0f, -25.0f,
                0.5f,    0.5f, 0.5f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/wall2.jpg");

  draw_a_object(20.0f,   150.0f, -100.0f,
                3.0f,    3.0f, 0.5f,
                1.0f,    0.0f, 0.0f,
                g_model,  180.0f, "./data/background4.jpg");
  draw_a_object(20.0f,   150.0f, 100.0f,
                3.0f,    3.0f, 0.5f,
                1.0f,    0.0f, 0.0f,
                g_model,  180.0f, "./data/background4.jpg");
  draw_a_object(-100.0f,   0.0f, -25.0f,
                5.0f,    3.0f, 0.5f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/leftBackground.jpg");
  draw_a_object(100.0f,   0.0f, -25.0f,
                5.0f,    3.0f, 0.5f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/leftBackground.jpg");
  draw_a_object(20.0f,   60.0f, -300.0f,
                15.0f,    15.0f, 1.0f,
                1.0f,    0.0f, 0.0f,
                g_model,  90.0f, "./data/sky2.jpg");

draw_a_object(20.0f,   0.0f, -300.0f,
              15.0f,    15.0f, 1.0f,
              1.0f,    0.0f, 0.0f,
              g_model,  90.0f, "./data/ground.jpg");

  draw_a_object(20.0f,   24.8f, 0.0f,
                0.5f,    0.5f, 0.5f,
                1.0f,    0.0f, 0.0f,
                g_model,  180.0f, "./data/door.jpg");
  draw_a_object(7.0f,   20.0f, -48.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    0.0f, 1.0f,
                g_model,  180.0f, "./data/picaso5.jpg"); // 정면 왼쪽 그림
  draw_a_object(32.0f,   20.0f, -48.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    0.0f, 1.0f,
                g_model,  180.0f, "./data/picaso2.jpg"); //정면 오른쪽 그림

  draw_a_object(44.0f,   10.0f, -38.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/gogh2.jpg"); // 오른쪽 안쪽 그림
  draw_a_object(43.0f,   11.0f, -25.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    1.0f, 0.0f,
                g_model,  90.0f, "./data/gogh.jpg"); //오른쪽 앞쪽 그림

  draw_a_object(-1.0f,   10.0f, -38.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    1.0f, 0.0f,
                g_model,  -90.0f, "./data/dali.jpg"); //왼쪽 안쪽 그림
  draw_a_object(-1.0f,   9.0f, -25.0f,
                0.08f,    0.2f, 0.1f,
                0.0f,    1.0f, 0.0f,
                g_model,  -90.0f, "./data/dali3.jpg"); //왼쪽 앞쪽 그림

  draw_a_object2(10.0f,   0.5f, -35.0f,
                3.0f,    3.0f, 3.0f,
                0.0f,    1.0f, 0.0f,
                g_platform,  0.0f);                 //왼쪽 단상

  draw_a_object2(27.0f,   0.5f, -35.0f,
                3.0f,    3.0f, 3.0f,
                0.0f,    1.0f, 0.0f,
                g_platform,  0.0f);                  //오른쪽 단상

  draw_a_object2(29.0f,   4.5f, -35.0f,
                3.0f,    3.0f, 3.0f,
                0.0f,    1.0f, 0.0f,
                g_sculpture,  0.0f);                  //오른쪽 조각상


  draw_a_object(20.0f,   22.0f, -30.0f,
                0.1f,    0.1f, 0.1f,
                0.0f,    1.0f, 0.0f,
                g_fan,  fan_angle, "./data/fan_texture2.jpg");


   draw_a_object2(40.0f,   5.0f, 5.0f,
                 1.0f,    1.0f, 1.0f,
                 0.0f,    1.0f, 0.0f,
                 g_giraffe,  0.0f);
	glUseProgram(0);

  glutSwapBuffers();
}

void draw_a_object(float tranX,   float tranY,   float tranZ,
                   float scaleX,  float scaleY,  float scaleZ,
                   float rotateX, float rotateY, float rotateZ,
                   Object object,     float angle, char* filePath)
{

  int width, height, channels;
  unsigned char* image = SOIL_load_image(filePath,
    &width, &height, &channels, SOIL_LOAD_RGB);

  // TODO: generate texture

  glBindTexture(GL_TEXTURE_2D, texid);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);

  SOIL_free_image_data(image);

  //create mat_Model
  kmuvcl::math::mat4x4f T,S,R;
  T = kmuvcl::math::translate(tranX, tranY, tranZ);
  S = kmuvcl::math::scale(scaleX, scaleY, scaleZ); //1은 z는 가만히 냅둔다라는 의미
  R = kmuvcl::math::rotate(angle, rotateX, rotateY, rotateZ);
  mat_Model = T*R*S;

  mat_PVM = mat_Proj*mat_View_inv*mat_Model;

  kmuvcl::math::mat3x3f mat_Normal;

  mat_Normal(0, 0) = mat_Model(0, 0);
  mat_Normal(0, 1) = mat_Model(0, 1);
  mat_Normal(0, 2) = mat_Model(0, 2);
  mat_Normal(1, 0) = mat_Model(1, 0);
  mat_Normal(1, 1) = mat_Model(1, 1);
  mat_Normal(1, 2) = mat_Model(1, 2);
  mat_Normal(2, 0) = mat_Model(2, 0);
  mat_Normal(2, 1) = mat_Model(2, 1);
  mat_Normal(2, 2) = mat_Model(2, 2);

  kmuvcl::math::mat4x4f mat_View = kmuvcl::math::inverse(mat_View_inv);

	glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);
  glUniformMatrix4fv(loc_u_model_matrix, 1, false, mat_Model);
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, mat_View);
  glUniformMatrix3fv(loc_u_normal_matrix, 1, false, mat_Normal);

  glUniform3fv(loc_u_light_vector, 1, light_vector);
  glUniform4fv(loc_u_light_ambient, 1, light_ambient);
  glUniform4fv(loc_u_light_diffuse, 1, light_diffuse);
  glUniform4fv(loc_u_light_specular, 1, light_specular);
  Shader::check_gl_error("glUniform4fv");

  glUniform1i(loc_u_texid, 0);

  object.draw(loc_a_vertex, loc_a_normal, loc_a_texcoord,
    loc_u_material_ambient, loc_u_material_diffuse,
    loc_u_material_specular, loc_u_material_shininess);
  Shader::check_gl_error("draw");

}

void draw_a_object2(float tranX,   float tranY,   float tranZ,
                   float scaleX,  float scaleY,  float scaleZ,
                   float rotateX, float rotateY, float rotateZ,
                   Object object,     float angle)
{
  //create mat_Model
  kmuvcl::math::mat4x4f T,S,R;
  T = kmuvcl::math::translate(tranX, tranY, tranZ);
  S = kmuvcl::math::scale(scaleX, scaleY, scaleZ); //1은 z는 가만히 냅둔다라는 의미
  R = kmuvcl::math::rotate(angle, rotateX, rotateY, rotateZ);
  mat_Model = T*R*S;

  mat_PVM = mat_Proj*mat_View_inv*mat_Model;

  kmuvcl::math::mat3x3f mat_Normal;

  mat_Normal(0, 0) = mat_Model(0, 0);
  mat_Normal(0, 1) = mat_Model(0, 1);
  mat_Normal(0, 2) = mat_Model(0, 2);
  mat_Normal(1, 0) = mat_Model(1, 0);
  mat_Normal(1, 1) = mat_Model(1, 1);
  mat_Normal(1, 2) = mat_Model(1, 2);
  mat_Normal(2, 0) = mat_Model(2, 0);
  mat_Normal(2, 1) = mat_Model(2, 1);
  mat_Normal(2, 2) = mat_Model(2, 2);

  kmuvcl::math::mat4x4f mat_View = kmuvcl::math::inverse(mat_View_inv);

	glUniformMatrix4fv(loc_u_pvm_matrix, 1, false, mat_PVM);
  glUniformMatrix4fv(loc_u_model_matrix, 1, false, mat_Model);
  glUniformMatrix4fv(loc_u_view_matrix, 1, false, mat_View);
  glUniformMatrix3fv(loc_u_normal_matrix, 1, false, mat_Normal);

  glUniform3fv(loc_u_light_vector, 1, light_vector);
  glUniform4fv(loc_u_light_ambient, 1, light_ambient);
  glUniform4fv(loc_u_light_diffuse, 1, light_diffuse);
  glUniform4fv(loc_u_light_specular, 1, light_specular);
  Shader::check_gl_error("glUniform4fv");

  glUniform1i(loc_u_texid, 0);

  object.draw(loc_a_vertex, loc_a_normal, loc_a_texcoord,
    loc_u_material_ambient, loc_u_material_diffuse,
    loc_u_material_specular, loc_u_material_shininess);
  Shader::check_gl_error("draw");

}

void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}

void keyboard(unsigned char key, int x, int y)
{

    if(key == 'a' || key == 'A')
  {
    g_camera.rotate_left(5.0f);
    glutPostRedisplay(); //os한테 다시 그려달라고 요청하는 것!  mydisplay를 호출하는게 아니다.
  }
  else if(key == 'd' || key == 'D')
  {
    g_camera.rotate_right(5.0f);
    glutPostRedisplay(); //os한테 다시 그려달라고 요청하는 것!  mydisplay를 호출하는게 아니다.
  }

  else if(key == 'w' || key == 'W') // 상승
  {
    float delta = 0.1f;
    g_camera.fly(delta);
    glutPostRedisplay();
  }
  else if(key == 's' || key == 'S') // 하강
  {
    float delta = -0.1f;
    g_camera.fly(delta);
    glutPostRedisplay();
  }
}

void special(int key, int x, int y)
{
	// TODO: properly handle special keyboard event
  switch(key)
      {
      case GLUT_KEY_UP:
          g_camera.move_forward(0.5f);
          break;
      case GLUT_KEY_DOWN:
          g_camera.move_backward(0.5f);
          break;
      case GLUT_KEY_LEFT:
          g_camera.move_left(0.5f);
          break;
      case GLUT_KEY_RIGHT:
          g_camera.move_right(0.5f);
          break;
      }

      glutPostRedisplay();
}

void idle()
{
  curr = std::chrono::system_clock::now();
  fan_angle += 3.0f;
  std::chrono::duration<float> elaped_seconds = (curr - prev);

  model_angle += 10 * elaped_seconds.count();
  prev = curr;

  glutPostRedisplay();
}
