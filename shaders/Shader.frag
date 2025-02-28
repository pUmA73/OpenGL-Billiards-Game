#version 330 core

in vec4 ex_Color;
out vec4 out_Color;

uniform vec4 ballColor;
uniform int codCol;

void main(void)
  {

    if(codCol == 0)
        out_Color = ex_Color;
    else 
        out_Color = ballColor;
  }
