//
//  sclarBar.js
//
//  Description:
//      Provides an object for rendering a 2D scalar color bar widget
//
//  Author:
//      Dan Ginsburg (daniel.ginsburg@childrens.harvard.edu)
//      Children's Hospital Boston
//


ScalarBar = function()
{
    this.minColor = new Float32Array(4);
    this.maxColor = new Float32Array(4);


    this.shaderProgram = null;

    // Initialize VBOs
    var vertices = [
             1.0,  1.0,  0.0, 0.0,
             0.0,  1.0,  0.0, 0.0,
             1.0,  0.0,  0.0, 1.0,
             0.0,  0.0,  0.0, 1.0
        ];

    this.vertexPositionBuffer = gl.createBuffer();        
    gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexPositionBuffer);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
    this.vertexPositionBuffer.itemSize = 4;
    this.vertexPositionBuffer.numItems = 4;



    // Initialize shaders
    this.fragShaderSrc =
        "#ifdef GL_ES\n" +
        "precision highp float;\n" +
        "#endif\n" +
        "uniform vec4 uMinColor;\n" +
        "uniform vec4 uMaxColor;\n" +
        "varying float vScalar;\n" +
        "void main(void) {\n" +
        "   gl_FragColor = vScalar * uMinColor + (1.0 - vScalar) * uMaxColor;\n" +
        "}\n";

    this.vertShaderSrc =
        "attribute vec4 aVertexPosition;\n" +
        
        "uniform mat4 uMVMatrix;\n" +
        "uniform mat4 uPMatrix;\n" +
        
        "varying float vScalar;\n" +
        "void main(void) {\n" +

        "  gl_Position = uPMatrix * uMVMatrix * vec4(aVertexPosition.xyz, 1.0);\n" +
        "  vScalar = aVertexPosition.w;\n" +
        "}\n";


    var fragmentShader = compileShader(this.fragShaderSrc, gl.FRAGMENT_SHADER);
    var vertexShader = compileShader(this.vertShaderSrc, gl.VERTEX_SHADER);

    this.shaderProgram = gl.createProgram();
    gl.attachShader(this.shaderProgram, vertexShader);
    gl.attachShader(this.shaderProgram, fragmentShader);
    gl.linkProgram(this.shaderProgram);

    if (!gl.getProgramParameter(this.shaderProgram, gl.LINK_STATUS)) {
        alert("Could not initialise shaders");
    }

    gl.useProgram(this.shaderProgram);

    this.shaderProgram.vertexPositionAttribute = gl.getAttribLocation(this.shaderProgram, "aVertexPosition");

    this.shaderProgram.pMatrixUniform = gl.getUniformLocation(this.shaderProgram, "uPMatrix");
    this.shaderProgram.mvMatrixUniform = gl.getUniformLocation(this.shaderProgram, "uMVMatrix");
    this.shaderProgram.minColorUniform = gl.getUniformLocation(this.shaderProgram, "uMinColor");
    this.shaderProgram.maxColorUniform = gl.getUniformLocation(this.shaderProgram, "uMaxColor");
}

// ScalarBar object
ScalarBar.prototype =
{
    // Set color range
    setColorRange: function(minColor, maxColor)
    {
        for(var i = 0; i < 4; i++)
        {
            this.minColor[i] = minColor[i];
            this.maxColor[i] = maxColor[i];
        }
        
    },
  
    // Draw the widget
    draw: function(pMatrix, mvMatrix)
    {
        gl.useProgram(this.shaderProgram);

        gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexPositionBuffer);
        gl.vertexAttribPointer(this.shaderProgram.vertexPositionAttribute,
                               this.vertexPositionBuffer.itemSize, gl.FLOAT, false, 0, 0);
        gl.enableVertexAttribArray(this.shaderProgram.vertexPositionAttribute);

        gl.uniformMatrix4fv(this.shaderProgram.pMatrixUniform, false, new Float32Array(pMatrix.flatten()));
        gl.uniformMatrix4fv(this.shaderProgram.mvMatrixUniform, false, new Float32Array(mvMatrix.flatten()));

        gl.uniform4fv(this.shaderProgram.minColorUniform, this.minColor);
        gl.uniform4fv(this.shaderProgram.maxColorUniform, this.maxColor);

        gl.disable(gl.CULL_FACE);
        gl.disable(gl.DEPTH_TEST);
        gl.disable(gl.BLEND);
        gl.drawArrays(gl.TRIANGLE_STRIP, 0, this.vertexPositionBuffer.numItems);
        
        gl.disableVertexAttribArray(this.shaderProgram.vertexPositionAttribute);
    }
}

