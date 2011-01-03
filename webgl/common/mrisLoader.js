//
//  mrisLoader.js
//
//  Description:
//      Provides a loader for FreeSurfer surface files
//      in JavaScript.  Ideas for this code were taken from three.js
//      at https://github.com/mrdoob/three.js.
//
//  Author:
//      Dan Ginsburg (daniel.ginsburg@childrens.harvard.edu)
//      Children's Hospital Boston
//


//  Represents the entire TrackFile, contains all of the
//  Tracks along with the data pre-processed for rendering
MRISFile = function()
{
    this.numVertices = 0;
    this.numFaces = 0;

    this.vertexPositions = null;
    this.vertexNormals = null;
    this.vertexNormalCount = null;
    this.vertexIndices = null;

    this.vertexPositionBuffer = null;
    this.vertexNormalBuffer = null;

    // Center of the object
    this.centerVect = new Float32Array(3);

    // Scale of the object
    this.scaleVect = new Float32Array(3);


}

MRISLoader = function()
{
}

// MRISLoader object
MRISLoader.prototype =
{
    // Load a MRIS file, provide a callback that handles loading
    // the data to WebGL
    load: function(mrisURL, callback)
    {
        var self = this;
        var xhr = new XMLHttpRequest();
        var length = 0;
        xhr.onreadystatechange = function()
        {
            if (xhr.readyState == 4)
            {
                if ( xhr.status == 200 || xhr.status == 0 )
                {
                    MRISLoader.prototype.loadMRISFile( xhr.responseText, callback );
                }
                else
                {
                    alert( "Couldn't load [" + url + "] [" + xhr.status + "]" );
                }
            }            
        }
        xhr.open("GET", mrisURL, true);
        xhr.overrideMimeType("text/plain; charset=x-user-defined");
        xhr.setRequestHeader("Content-Type", "text/plain");
        xhr.send(null);
    },

    // Internal function, initiates loading and processing the MRIS file
    loadMRISFile: function(data, callback)
    {
        var mrisFile = new MRISFile();
        var currentOffset = 0;
        
        var magic = parseString( data, currentOffset, 3 );
        currentOffset += 3;

        // Go through two newlines
        var iters = 0;
        var curChar;
        do
        {
            curChar = parseUChar8( data, currentOffset++ );
            iters++;
        }
        while ((iters < 200) && (curChar != 0x0A))

        // Read one more newline
        curChar = parseUChar8( data, currentOffset++ );
        
        mrisFile.numVertices = parseUInt32EndianSwapped( data, currentOffset );
        currentOffset += 4;
        console.log('Vertices: ' + mrisFile.numVertices)

        mrisFile.numFaces = parseUInt32EndianSwapped( data, currentOffset );
        currentOffset += 4;

        mrisFile.vertexPositions = new Float32Array( mrisFile.numVertices * 3 );
        mrisFile.vertexNormals = new Float32Array( mrisFile.numVertices * 3 );
        mrisFile.vertexNormalCount = new Int32Array( mrisFile.numVertices );
        for (var v = 0; v < mrisFile.numVertices; v++)
        {
            mrisFile.vertexPositions[v * 3 + 0] = parseFloat32EndianSwapped( data, currentOffset );
            mrisFile.vertexNormals[v * 3 + 0] = 0.0;
            currentOffset += 4;
            mrisFile.vertexPositions[v * 3 + 1] = parseFloat32EndianSwapped( data, currentOffset );
            mrisFile.vertexNormals[v * 3 + 1] = 0.0;
            currentOffset += 4;
            mrisFile.vertexPositions[v * 3 + 2] = parseFloat32EndianSwapped( data, currentOffset );
            mrisFile.vertexNormals[v * 3 + 2] = 0.0;            
            currentOffset += 4;

            mrisFile.vertexNormalCount[v] = 0;
        }

        // Read teh faces and compute the average vertex normal for each vertex
        mrisFile.vertexIndices = new Int32Array( mrisFile.numFaces * 3 );
        for (var f = 0; f < mrisFile.numFaces; f++)
        {
            for (var n = 0; n < 3; n++)
            {
                mrisFile.vertexIndices[f * 3 + n] = parseUInt32EndianSwapped( data, currentOffset );
                mrisFile.vertexNormalCount[mrisFile.vertexIndices[f * 3 + n]] += 1;
                currentOffset += 4;
            }

            var index;

            // Compute the face normal
            index = mrisFile.vertexIndices[f * 3 + 0];
            var v0 = Vector.create([mrisFile.vertexPositions[(index * 3) + 0],
                                    mrisFile.vertexPositions[(index * 3) + 1],
                                    mrisFile.vertexPositions[(index * 3) + 2]]);

            index = mrisFile.vertexIndices[f * 3 + 1];
            var v1 = Vector.create([mrisFile.vertexPositions[(index * 3) + 0],
                                    mrisFile.vertexPositions[(index * 3) + 1],
                                    mrisFile.vertexPositions[(index * 3) + 2]]);

            index = mrisFile.vertexIndices[f * 3 + 2];
            var v2 = Vector.create([mrisFile.vertexPositions[(index * 3) + 0],
                                    mrisFile.vertexPositions[(index * 3) + 1],
                                    mrisFile.vertexPositions[(index * 3) + 2]]);
            
            var n0 = v1.subtract(v0);
            var n1 = v2.subtract(v1);

            var normal = n0.cross(n1).toUnitVector();

            // Add the face normal to each of the three vertices
            index = mrisFile.vertexIndices[f * 3 + 0];
            mrisFile.vertexNormals[(index * 3) + 0] += normal.elements[0];
            mrisFile.vertexNormals[(index * 3) + 1] += normal.elements[1];
            mrisFile.vertexNormals[(index * 3) + 2] += normal.elements[2];

            index = mrisFile.vertexIndices[f * 3 + 1];
            mrisFile.vertexNormals[(index * 3) + 0] += normal.elements[0];
            mrisFile.vertexNormals[(index * 3) + 1] += normal.elements[1];
            mrisFile.vertexNormals[(index * 3) + 2] += normal.elements[2];

            index = mrisFile.vertexIndices[f * 3 + 2];
            mrisFile.vertexNormals[(index * 3) + 0] += normal.elements[0];
            mrisFile.vertexNormals[(index * 3) + 1] += normal.elements[1];
            mrisFile.vertexNormals[(index * 3) + 2] += normal.elements[2];
        }

        // Now average all the normals
        for (v = 0; v < mrisFile.numVertices; v++)
        {
            mrisFile.vertexNormals[(v * 3) + 0] /= mrisFile.vertexNormalCount[v];
            mrisFile.vertexNormals[(v * 3) + 1] /= mrisFile.vertexNormalCount[v];
            mrisFile.vertexNormals[(v * 3) + 2] /= mrisFile.vertexNormalCount[v];
        }

        this.preprocessForRendering(mrisFile);
        
        callback(mrisFile);
    },


    // After loading the MRIS file, preprocess the data into
    // arrays that can be used for rendering it using WebGL
    preprocessForRendering: function (mrisFile)
    {
        console.log('Begin postprocessing...');
        var vertIdx = 0;
        var elemIdx = 0;

        mrisFile.vertexPositionBuffer = new Float32Array(mrisFile.numFaces * 3 * 3);
        mrisFile.vertexNormalBuffer = new Float32Array(mrisFile.numFaces * 3 * 3);
        console.log('Allocated memory.');

        var min = [Number.MAX_VALUE, Number.MAX_VALUE, Number.MAX_VALUE];
        var max = [Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE];

        var index = 0;
        var normalIndex = 0;
        for (var f = 0; f < mrisFile.numFaces; f++)
        {
            for (var n = 0; n < 3; n++)
            {
                for (var p = 0; p < 3; p++)
                {
                    mrisFile.vertexPositionBuffer[index] =
                        mrisFile.vertexPositions[(mrisFile.vertexIndices[f * 3 + n] * 3) + p];
                    mrisFile.vertexNormalBuffer[index] =
                        mrisFile.vertexNormals[(mrisFile.vertexIndices[f * 3 + n] * 3) + p];
                    
                    if (min[p] > mrisFile.vertexPositionBuffer[index] )
                        min[p] = mrisFile.vertexPositionBuffer[index];
                    if (max[p] < mrisFile.vertexPositionBuffer[index] )
                        max[p] = mrisFile.vertexPositionBuffer[index];
                    
                    index++;
                }

            }
        }

        for (var idx = 0; idx < 3; idx++)
        {
            mrisFile.centerVect[idx] = (max[idx] - min[idx]) / 2.0 + min[idx];
            mrisFile.scaleVect[idx] = 1.0 / (max[idx] - min[idx]);
        }
    }
}
