//
//  trkLoader.js
//
//  Description:
//      Provides a loader for TrackVis (http://www.trackvis.org) 'trk' files
//      in JavaScript.  Ideas for this code were taken from three.js
//      at https://github.com/mrdoob/three.js.
//
//  Author:
//      Dan Ginsburg (daniel.ginsburg@childrens.harvard.edu)
//      Children's Hospital Boston
//

//  Represents a point with possible scalars from the 'trk' file
TrackPoint = function(x, y, z, scalars)
{
    this.position = [ x, y, z];
    this.scalars = scalars;
}

//  Represents a single individual track, with an array
//  of TrackPoints and optional properties
Track = function(numPoints, points, properties)
{
    // Number of points in this track
    this.numPoints = numPoints;

    // Array of TrackPoint's'
    this.points = points;

    // Optional properties array
    this.properties = properties;
}

//  Represents the entire TrackFile, contains all of the
//  Tracks along with the data pre-processed for rendering
TrackFile = function()
{
    // Data loaded directly from 'trk' file'
    this.trkHeader = null;
    this.trkTracks = null;

    // Total Number of vertices
    this.numVertices = 0;
    
    // Center of the object
    this.centerVect = new Float32Array(3);

    // Scale of the object
    this.scaleVect = new Float32Array(3);

    // Computed data needed for rendering using
    // index GL_LINES primitives
    this.vertexPositionBuffer = null;
    this.vertexColorBuffer = null;
}

TrkLoader = function()
{
}

// TrkLoader object
TrkLoader.prototype =
{
    // Load a 'trk' file, provide a callback that handles loading
    // the data to WebGL
    load: function(trkURL, callback)
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
                    TrkLoader.prototype.loadTrkFile( xhr.responseText, callback );
                }
                else
                {
                    alert( "Couldn't load [" + url + "] [" + xhr.status + "]" );
                }
            }            
        }
        xhr.open("GET", trkURL, true);
        xhr.overrideMimeType("text/plain; charset=x-user-defined");
        xhr.setRequestHeader("Content-Type", "text/plain");
        xhr.send(null);
    },

    // Internal function, initiates loading and processing the 'trk' file
    loadTrkFile: function(data, callback)
    {
        var trackFile = new TrackFile();

        var currentOffset = 0;

        trackFile.trkHeader = this.loadTrkHeader(data, currentOffset);
        currentOffset += trackFile.trkHeader.hdr_size;
        trackFile.trkTracks = this.loadTrkStreams(trackFile, data, currentOffset,
                                                  trackFile.trkHeader);

        // Generate all of the data needed for rendering
        this.preprocessForRendering(trackFile);

        callback(trackFile);
    },

    // Load the 'trk' file header
    loadTrkHeader: function (data, offset)
    {
        var header =
        {
            'id_string'                 :   parseString( data, offset, 6),
            'dim'                       :   parseUInt16Array( data, offset + 6, 3),
            'voxel_size'                :   parseFloat32Array( data, offset + 12, 3),
            'origin'                    :   parseFloat32Array( data, offset + 24, 3),
            'n_scalars'                 :   parseUInt16( data, offset + 36),
            'scalar_name'               :   parseString( data, offset + 40, 200),
            'n_properties'              :   parseUInt16( data, offset + 240),
            'property_name'             :   parseString( data, offset + 242, 200),
            'vox_to_ras'                :   parseFloat32Array( data, offset + 442, 16),
            'reserved'                  :   parseString( data, offset + 506, 444),
            'voxel_order'               :   parseString( data, offset + 950, 4),
            'pad2'                      :   parseString( data, offset + 954, 2),
            'image_orientation_patient' :   parseFloat32Array( data, offset + 956, 6),
            'pad1'                      :   parseString( data, offset + 980, 2),
            'invert_x'                  :   parseUChar8( data, offset + 982),
            'invert_y'                  :   parseUChar8( data, offset + 983),
            'invert_z'                  :   parseUChar8( data, offset + 984),
            'swap_xy'                   :   parseUChar8( data, offset + 985),
            'swap_yz'                   :   parseUChar8( data, offset + 986),
            'swap_zx'                   :   parseUChar8( data, offset + 987),
            'n_count'                   :   parseUInt32( data, offset + 988),
            'version'                   :   parseUInt32( data, offset + 992),
            'hdr_size'                  :   parseUInt32( data, offset + 996)
        };

        return header;

    },

    // Load all of the 'trk' file streams
    loadTrkStreams: function (trackFile, data, offset, header)
    {
        var totalPoints = 0;
        var totalSegments = 0;

        var tracks = new Array(header.n_count);
        for( var i = 0; i < header.n_count; i++ )
        {
            var numPoints = parseUInt32(data, offset);
            offset += 4;
            var pointArray = new Array(numPoints);
            totalPoints += numPoints;
            totalSegments += (numPoints - 1)
            for ( var j = 0; j < numPoints; j++ )
            {
                var x = parseFloat32(data, offset);
                offset += 4;
                var y = parseFloat32(data, offset);
                offset += 4;
                var z = parseFloat32(data, offset);
                offset += 4;
                var scalars = null;

                if (header.n_scalars > 0)
                {
                    scalars = parseFloat32Array(data, offset, header.n_scalars);
                    offset += (header.n_scalars * 4);
                }

                pointArray[j] = new TrackPoint(x, y, z, scalars);
            }

            var properties = null;
            if (header.n_properties > 0)
            {
                properties = parseFloat32Array(data, offset, header.n_properties);
                offset += (header.n_properties * 4);
            }
            tracks[i] = new Track(numPoints, pointArray, properties)
        }

        trackFile.numVertices = (totalSegments * 2);
        
        return tracks;
    },

    // After loading the 'trk' file, preprocess the data into
    // arrays that can be used for rendering it using WebGL
    preprocessForRendering: function (trackFile)
    {
        console.log('Begin postprocessing...');
        var vertIdx = 0;
        var elemIdx = 0;

        trackFile.vertexPositionBuffer = new Float32Array(trackFile.numVertices * 3);
        trackFile.vertexColorBuffer = new Float32Array(trackFile.numVertices * 3);
        console.log('Allocated memory.');


        var min = [Number.MAX_VALUE, Number.MAX_VALUE, Number.MAX_VALUE];
        var max = [Number.MIN_VALUE, Number.MIN_VALUE, Number.MIN_VALUE];

        var numTracks = trackFile.trkHeader.n_count;

        for (var i = 0; i < numTracks; i++)
        {
            var numPoints = trackFile.trkTracks[i].numPoints;

            var startVert = trackFile.trkTracks[i].points[0].position;
            var endVert = trackFile.trkTracks[i].points[numPoints-1].position;
            var diffVector = [ Math.abs(endVert[0] - startVert[0]),
                               Math.abs(endVert[1] - startVert[1]),
                               Math.abs(endVert[2] - startVert[2]) ];

            var len = Math.sqrt(diffVector[0] * diffVector[0] +
                                diffVector[1] * diffVector[1] +
                                diffVector[2] * diffVector[2]);

            diffVector[0] /= len;
            diffVector[1] /= len;
            diffVector[2] /= len;


            for (var p = 0; p < numPoints-1; p++)
            {
                for (var j = 0; j < 3; j++)
                {
                    if (min[j] > trackFile.trkTracks[i].points[p].position[j] )
                        min[j] = trackFile.trkTracks[i].points[p].position[j];
                    if (max[j] < trackFile.trkTracks[i].points[p].position[j] )
                        max[j] = trackFile.trkTracks[i].points[p].position[j];
                }

                // Add a vertex point for each end of the line segment.  I would
                // use an index buffer instead, but it looks to me that only
                // ushort indices are supported so I would need to break the
                // geometry up into chunks because we often have more than 2^16
                // points
                for (var seg = 0; seg < 2; seg++)
                {
                    trackFile.vertexPositionBuffer[3 * vertIdx + 0] = trackFile.trkTracks[i].points[p+seg].position[0];
                    trackFile.vertexPositionBuffer[3 * vertIdx + 1] = trackFile.trkTracks[i].points[p+seg].position[1];
                    trackFile.vertexPositionBuffer[3 * vertIdx + 2] = trackFile.trkTracks[i].points[p+seg].position[2];
                    trackFile.vertexColorBuffer[3 * vertIdx + 0] = diffVector[0];
                    trackFile.vertexColorBuffer[3 * vertIdx + 1] = diffVector[1];
                    trackFile.vertexColorBuffer[3 * vertIdx + 2] = diffVector[2];
                    vertIdx++;
                }                               
            }
        }

        console.log('Created vertex buffers.');

        for (var idx = 0; idx < 3; idx++)
        {
            trackFile.centerVect[idx] = (max[idx] - min[idx]) / 2.0 + min[idx];
            trackFile.scaleVect[idx] = 1.0 / (max[idx] - min[idx]);
        }

        console.log('Done.');
    }

}
