/********************************************************
 * Steven Hohs                                          *
 * CS 535                                               *
 * Project #6                                           *
 * blending.js                                          *
 * This program shows an Alabama A, which is semitrans- *
 * parent. The user can invert the transparency with    *
 * toggle button, and can move the A around the screen  *
 * with the other buttons.                              *
 ********************************************************/

var canvas;
var gl;

var vertices = [];
var texCoords = [];
var tex;

window.onload = function init() {
  var canvas = document.getElementById("gl-canvas");

  gl = WebGLUtils.setupWebGL(canvas);
  if (!gl) { alsert ("WebGL isn't available"); }

  gl.viewport(0, 0, canvas.width, canvas.height);
  gl.clearColor(1.0, 1.0, 1.0, 1.0);

  var program = initShaders(gl, "vertex-shader", "fragment-shader");
  gl.useProgram(program);
  
  var adjw = nextPow2(WIDTH);
  var adjh = nextPow2(HEIGHT);

  var texsize = Math.max(adjw, adjh);

  quad(vec4(-1.0,  1.0, 0.0, 1.0),
       vec4(-1.0, -1.0, 0.0, 1.0),
       vec4( 1.0, -1.0, 0.0, 1.0),
       vec4( 1.0,  1.0, 0.0, 1.0),
       vec2(0.0, 0.0),
       vec2(0.0, HEIGHT / texsize),
       vec2(WIDTH / texsize, HEIGHT / texsize),
       vec2(WIDTH / texsize, 0.0));

  tex = gl.createTexture();
  var texColors = [];
  
  var i = 0;


  for (var h = 0; h < HEIGHT; h++) {
    for (var w = 0; w < WIDTH; w++) {
      var r = (texture[i] <= 0.5) ? 255 : 255 - 511 * texture[i];
      var g = (texture[i] >= 0.5) ? 255 : 511 * texture[i];
      texColors.push(r, g, 0.0, 255);
      i++;
    }

    for (var w = WIDTH; w < texsize; w++) {
      texColors.push(0.0, 0.0, 0.0, 255);
    }
  }

  for (var h = HEIGHT; h < texsize; h++) {
    for (var w = 0; w < texsize; w++) {
      texColors.push(0.0, 0.0, 0.0, 255);
    }
  }


  gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, texsize, texsize, 0, gl.RGBA, gl.UNSIGNED_BYTE, new Uint8Array(texColors));
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
  gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
  
  var vBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, flatten(vertices), gl.STATIC_DRAW);

  var vPosition = gl.getAttribLocation(program, "vPosition");
  gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(vPosition);

  var tBuffer = gl.createBuffer();
  gl.bindBuffer(gl.ARRAY_BUFFER, tBuffer);
  gl.bufferData(gl.ARRAY_BUFFER, flatten(texCoords), gl.STATIC_DRAW);

  var vTexCoord = gl.getAttribLocation(program, "vTexCoord");
  gl.vertexAttribPointer(vTexCoord, 2, gl.FLOAT, false, 0, 0);
  gl.enableVertexAttribArray(vTexCoord);

  render();
}

function nextPow2(a) {
  var power = 1;
  while (power < a) {
    power *= 2;
  }
  return power;
}

function quad(v0, v1, v2, v3, t0, t1, t2, t3) {
  vertices.push(v0, v1, v2, v2, v3, v0);
  texCoords.push(t0, t1, t2, t2, t3, t0);
}


function render() {

  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

  // gl.bindTexture(gl.TEXTURE_2D, tex);
  gl.drawArrays(gl.TRIANGLES, 0, 6);

  requestAnimFrame(render);
}