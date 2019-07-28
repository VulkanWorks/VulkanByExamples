#include "Transformation3D.h"

/*!
 Constructor for Transformation3D class
 \param[in]  void.
 \return     None.
 */
Transformation3D::Transformation3D(void)
{
	TransformMemData.modelMatrixIndex       = 0;
	TransformMemData.viewMatrixIndex        = 0;
	TransformMemData.projectionMatrixIndex	= 0;
	TransformMemData.textureMatrixIndex		= 0;
}

/*!
    Destructor for Transformation3D class
    \param[in]  void.
    \return     None.
*/
Transformation3D::~Transformation3D(void)
{
}

/*!
	Initialize the global  structure and set all the default OpenGLES machine
	states to their default values as expected by the engine. In addition the matrix
	stacks are created and the first index of the model view, projection and texture
	matrix are set to identity.
    \param[in]  None.
    \return void.
 
*/
void Transformation3D::Init( void )
{
	memset( &TransformMemData, 0, sizeof( TransformData ) );

	SetMatrixMode( TEXTURE_MATRIX );
	LoadIdentity();

	SetMatrixMode( PROJECTION_MATRIX );
	LoadIdentity();

	SetMatrixMode( MODEL_MATRIX );
	LoadIdentity();

	SetMatrixMode( VIEW_MATRIX );
	LoadIdentity();
}

/*!
	Set the current matrix mode that you want to work with. Only the MODELVIEW_MATRIX,
	PROJECTION_MATRIX and TEXTURE_MATRIX are supported. Take note that contrairly to the default OpenGL behavior
	the TEXTURE_MATRIX is not implemented on a per channel basis, but on a global basis) contain in .h.

	If you wish to tweak the default matrix stack depth you can modify the following values: MAX_MODELVIEW_MATRIX,
	MAX_PROJECTION_MATRIX, MAX_TEXTURE_MATRIX.

	\param[in] mode The matrix mode that will be use as target.
*/
void Transformation3D::SetMatrixMode( unsigned int mode )
{
    TransformMemData.matrix_mode = mode;
}

/*!
	Set the current matrix set as target by the _set_matrix_mode to the
	identity matrix.
 \param[in]  None.
 \return None.
 
*/
void Transformation3D::LoadIdentity( void )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			memset(GetModelMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* mv = GetModelMatrix();
			(*mv)[0][0] = (*mv)[1][1] = (*mv)[2][2] = (*mv)[3][3] = 1.0;
			break;
		}
            
		case VIEW_MATRIX:
		{
			memset(GetViewMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* mv = GetViewMatrix();
			(*mv)[0][0] = (*mv)[1][1] = (*mv)[2][2] = (*mv)[3][3] = 1.0;
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			memset(GetProjectionMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* pm = GetProjectionMatrix();
			(*pm)[0][0] = (*pm)[1][1] = (*pm)[2][2] = (*pm)[3][3] = 1.0;
			break;
		}

		case TEXTURE_MATRIX:
		{
			memset(GetTextureMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* tm = GetTextureMatrix();
			(*tm)[0][0] = (*tm)[1][1] = (*tm)[2][2] = (*tm)[3][3] = 1.0;
			break;
		}
	}
}

/*!
 Return the modelview matrix pointer on the top of the modelview matrix stack.
 \param[in]  None.
 \return Return a 4x4 matrix pointer of the top most modelview matrix.
 */
glm::mat4* Transformation3D::GetModelMatrix( void )
{
	return &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex ];
}


/*!
 Return the modelview matrix pointer on the top of the modelview matrix stack.
 \param[in]  None.
 \return Return a 4x4 matrix pointer of the top most modelview matrix.
 */
glm::mat4* Transformation3D::GetViewMatrix( void )
{
	return &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex ];
}

/*!
	Return the projection matrix pointer on the top of the projection matrix stack.
    \param[in]  None.
	\return  Return a 4x4 matrix pointer of the top most projection matrix index.
*/
glm::mat4* Transformation3D::GetProjectionMatrix( void )
{
	return &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex ];
}

/*!
	Return the texture matrix pointer on the top of the texture matrix stack.
    \param[in]  None.
	\return  Return a 4x4 matrix pointer of the top most texture matrix index.
*/
glm::mat4* Transformation3D::GetTextureMatrix( void )
{
	return &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex ];
}

/*!
 Return the ModelViewProjection matrix. 
 \param[in]  None.
 \return matrix- Model View Projection matrix multiplication result.

 */
glm::mat4* Transformation3D::GetModelViewProjectionMatrix( void )
{
	TransformMemData.modelview_projection_matrix = *GetProjectionMatrix() *
    *GetViewMatrix() * *GetModelMatrix();
	return &TransformMemData.modelview_projection_matrix;
}

/*!
 Return the ModelView matrix.
 \param[in]  None.
 \return matrix- Model View matrix multiplication result.
 
*/
glm::mat4* Transformation3D::GetModelViewMatrix( void )
{
	TransformMemData.modelview_matrix =
    *GetViewMatrix() * *GetModelMatrix();
	return &TransformMemData.modelview_matrix;
}

/*!
 Pushes the current matrix on the Matrix stack as per current active matrix.

 \param[in]  None.
 \return None.
 */
void Transformation3D::PushMatrix( void )
{
	switch( TransformMemData.matrix_mode )
	{
        case MODEL_MATRIX:
		{
			if (TransformMemData.modelMatrixIndex >= MAX_MODEL_MATRIX){
				return;
			}
            
			memcpy( &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex + 1 ],
                   &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex     ],
                   sizeof( glm::mat4 ) );
            
			++TransformMemData.modelMatrixIndex;
            
			break;
		}

		case VIEW_MATRIX:
		{
			if (TransformMemData.viewMatrixIndex >= MAX_VIEW_MATRIX){
				return;
			}
            
			memcpy( &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex + 1 ],
                   &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex     ],
                   sizeof( glm::mat4 ) );
            
			++TransformMemData.viewMatrixIndex;
            
			break;
		}

		case PROJECTION_MATRIX:
		{
			if (TransformMemData.projectionMatrixIndex >= MAX_PROJECTION_MATRIX){
				return;
			}

			memcpy( &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex + 1 ],
                    &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex     ],
					sizeof( glm::mat4 ) );

			++TransformMemData.projectionMatrixIndex;

			break;
		}

		case TEXTURE_MATRIX:
		{
			if (TransformMemData.textureMatrixIndex >= MAX_TEXTURE_MATRIX){
				return;
			}

			memcpy( &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex + 1 ],
                    &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex     ],
					sizeof( glm::mat4 ) );

			++TransformMemData.textureMatrixIndex;
			break;
		}
	}
}

/*!
 Pop's the current matrix on the Matrix stack as per current active matrix.

 \param[in]  None.
 \return None.

 */
void Transformation3D::PopMatrix( void )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			if (TransformMemData.modelMatrixIndex == 0){
				return;
			}
			--TransformMemData.modelMatrixIndex;
            
			break;
		}

		case VIEW_MATRIX:
		{
			if (TransformMemData.viewMatrixIndex == 0){
				return;
			}
			--TransformMemData.viewMatrixIndex;
            
			break;
		}

		case PROJECTION_MATRIX:
		{
			if (TransformMemData.projectionMatrixIndex == 0){
				return;
			}
			--TransformMemData.projectionMatrixIndex;

			break;
		}

		case TEXTURE_MATRIX:
		{
			if (TransformMemData.textureMatrixIndex == 0){
				return;
			}
			--TransformMemData.textureMatrixIndex;

			break;
		}
	}
}

/*!
 Set the matrix sent as the current matrix.
 
 \param[in] m Specify matrix which needs to set current.
 
 */
void Transformation3D::LoadMatrix( glm::mat4 *m )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			memcpy(GetModelMatrix(), m, sizeof(glm::mat4));
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			memcpy(GetViewMatrix(), m, sizeof(glm::mat4));
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			memcpy(GetProjectionMatrix(), m, sizeof(glm::mat4));

			break;
		}

		case TEXTURE_MATRIX:
		{
			memcpy(GetTextureMatrix(), m, sizeof(glm::mat4));

			break;
		}
	}
}

/*!
 Multiple the matrix with the current matrix.
 
 \param[in] m Specify matrix which needs to multiply.
 
 */
void Transformation3D::MultiplyMatrix( glm::mat4 *m )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*GetModelMatrix() *=  (*m);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*GetViewMatrix() *=  (*m);
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*GetProjectionMatrix() *=  (*m);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() *=  (*m);

			break;
		}
	}
}

/*!
	Translate the current matrix pointed for the current mode set
	as target by the _set_matrix_mode function.

	\param[in] x Specify the x coordinate of a translation vector.
	\param[in] y Specify the y coordinate of a translation vector.
	\param[in] z Specify the z coordinate of a translation vector.

*/
void Transformation3D::Translate( float x, float y, float z )
{
	glm::vec3 v( x, y, z );

	switch( TransformMemData.matrix_mode )
	{            
		case MODEL_MATRIX:
		{
			*GetModelMatrix() = glm::translate( *GetModelMatrix(), v);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*GetViewMatrix() = glm::translate( *GetViewMatrix(), v);
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*GetProjectionMatrix() = glm::translate( *GetProjectionMatrix(), v);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() = glm::translate( *GetTextureMatrix(), v);

			break;
		}
	}
}

/*!
	Rotate the current matrix pointed for the current mode.

	\param[in] angle Specifies the angle of rotation, in degrees.
	\param[in] x Specify the x coordinate of a vector.
	\param[in] y Specify the y coordinate of a vector.
	\param[in] z Specify the z coordinate of a vector.

*/
void Transformation3D::Rotate( float angle, float x, float y, float z )
{
	if( !angle ) return;

	glm::vec3 v( x, y, z);

	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*GetModelMatrix() = glm::rotate( *GetModelMatrix(), angle, v );
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*GetViewMatrix() = glm::rotate( *GetViewMatrix(), angle, v );
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*GetProjectionMatrix() = glm::rotate( *GetProjectionMatrix(), angle, v );

			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() = glm::rotate( *GetTextureMatrix(), angle, v );

			break;
		}
	}
}

/*!
	Scale the current matrix pointed for the current mode.

	\param[in] x Specify scale factor along the x axis.
	\param[in] y Specify scale factor along the y axis.
	\param[in] z Specify scale factor along the z axis.

*/
void Transformation3D::Scale( float x, float y, float z )
{
	static glm::vec3 scale( 1.0f, 1.0f, 1.0f );

	glm::vec3 v( x, y, z );

	if( !memcmp( &v, &scale, sizeof( glm::vec3 ) ) ) return;


	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*GetModelMatrix() = glm::scale(*GetModelMatrix(), v);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*GetViewMatrix() = glm::scale(*GetViewMatrix(), v);
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*GetProjectionMatrix() = glm::scale(*GetProjectionMatrix(), v);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() = glm::scale(*GetTextureMatrix(), v);

			break;
		}
	}
}

/*!
	Return the result of the inverse and transposed operation of the top most modelview matrix applied
	on the rotation part of the matrix.

	\return Return the 3x3 matrix pointer that represent the invert and transpose
	result of the top most model view matrix.
*/
void Transformation3D::GetNormalMatrix( glm::mat3* mat3Obj )
{
	glm::mat4 mat4Obj;

	mat4Obj = glm::inverse(*GetModelViewMatrix());
	mat4Obj = glm::transpose(*GetModelViewMatrix());

	memcpy(&(*mat3Obj)[0], &mat4Obj[0], sizeof(glm::mat3));
	memcpy(&(*mat3Obj)[1], &mat4Obj[0], sizeof(glm::mat3));
	memcpy(&(*mat3Obj)[2], &mat4Obj[0], sizeof(glm::mat3));
}


/*!
	Multiply the current matrix with an orthographic matrix.

	\param[in] left Specify the coordinates for the left vertical clipping planes.
	\param[in] right Specify the coordinates for the right vertical clipping planes.
	\param[in] bottom Specify the coordinates for the bottom horizontal clipping planes.
	\param[in] top Specify the coordinates for the top horizontal clipping planes.
	\param[in] clip_start Specify the distance to the nearer depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] clip_end Specify the distance to the farther depth clipping planes. This value is negative if the plane is to be behind the viewer.

*/
void Transformation3D::Ortho( float left, float right, float bottom, float top, float clip_start, float clip_end )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*GetModelMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}
		
        case VIEW_MATRIX:
		{
			*GetViewMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}

		case PROJECTION_MATRIX:
		{
			*GetProjectionMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}
	}
}

/*!
	Multiply the current matrix with a scaled orthographic matrix that respect the current
	screen and and aspect ratio.

	\param[in] screen_ratio Specifies the screen ratio that determines the field of view in the Y direction. The screen ratio is the ratio of y (height) to y (width).
	\param[in] scale Determine that scale value that should be applied to the matrix.
	\param[in] aspect_ratio Specifies the aspect ratio that determines the field of view in the X direction. The screen ratio is the ratio of x (width) to y (height).
	\param[in] clip_start Specify the distance to the nearer depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] clip_end Specify the distance to the farther depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] screen_orientation A value in degree that rotate the matrix. Should be synchronized with your device orientation.
*/
void Transformation3D::OrthoGrahpic( float screen_ratio, float scale, float aspect_ratio, float clip_start, float clip_end, float orientation )
{
	scale = ( scale * 0.5f ) * aspect_ratio;

	Ortho( -1.0f, 1.0f, -screen_ratio, screen_ratio, clip_start, clip_end );
	Scale( 1.0f / scale, 1.0f / scale, 1.0f );

	if ( orientation ) {
        Rotate( orientation, 0.0f, 0.0f, 1.0f );
    }
}

/*!
	Set up a perspective projection matrix.

	\param[in] fovy Specifies the field of view angle, in degrees, in the y direction.
	\param[in] aspect_ratio Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
	\param[in] clip_start Specifies the distance from the viewer to the near clipping plane (always positive).
	\param[in] clip_end Specifies the distance from the viewer to the far clipping plane (always positive).

*/
void Transformation3D::SetPerspective( float fovy, float aspect_ratio, float clip_start, float clip_end )
{
	glm::mat4 mat;

	switch( TransformMemData.matrix_mode )
	{            
		case MODEL_MATRIX:
		{
			*GetModelMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
			break;
		}
            
		case VIEW_MATRIX:
		{
			*GetViewMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
			break;
		}
            
		case PROJECTION_MATRIX:
		{
//			glm::mat4	*matProjection = GetProjectionMatrix();
			*GetProjectionMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
	//		*matProjection = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*GetTextureMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);

			break;
		}
	}

}

void Transformation3D::SetView(glm::mat4 mat)
{
	//TransformMemData.view_matrix = mat;
}

/*!
	Define a viewing transformation.

	\param[in] eye Specifies the position of the eye point.
	\param[in] center Specifies the position of the reference point.
	\param[in] up Specifies the direction of the up vector.

*/
void Transformation3D::LookAt( glm::vec3 *eye, glm::vec3 *center, glm::vec3 *up )
{
	*GetViewMatrix() = glm::lookAt(*eye, *center, *up);

	//glm::vec3 f, s, u;
	//static glm::mat4 mat = glm::lookAt(*eye, *center, *up);
	////TransformSetView(mat);
	////*GetViewMatrix() = (mat);
	//MultiplyMatrix( &mat );
}

/*!
	Map object coordinates to window coordinates.

	\param[in] objextx Specify the object X coordinate.
	\param[in] objecty Specify the object Y coordinate.
	\param[in] objectz Specify the object Z coordinate.
	\param[in] modelview_matrix Specifies the current modelview matrix.
	\param[in] projection_matrix Specifies the current projection matrix.
	\param[in] viewport_matrix Specifies the current viewport matrix.
	\param[in] windowx Return the computed X window coordinate.
	\param[in] windowy Return the computed Y window coordinate.
	\param[in] windowz Return the computed Z window coordinate.

*/
int Transformation3D::TransformProject( float objextx, float objecty, float objectz, glm::mat4 *modelview_matrix, glm::mat4 *projection_matrix, int *viewport_matrix, float *windowx, float *windowy, float *windowz )
{    
	glm::vec4 vin,
		 vout;

	vin.x = objextx;
	vin.y = objecty;
	vin.z = objectz;
	vin.w = 1.0f;

	Vec4MultiplyMat4( &vout, &vin, modelview_matrix );

	Vec4MultiplyMat4( &vin, &vout, projection_matrix );

	if( !vin.w ) return 0;

	vin.x /= vin.w;
	vin.y /= vin.w;
	vin.z /= vin.w;

	vin.x = vin.x * 0.5f + 0.5f;
	vin.y = vin.y * 0.5f + 0.5f;
	vin.z = vin.z * 0.5f + 0.5f;

	vin.x = vin.x * viewport_matrix[ 2 ] + viewport_matrix[ 0 ];
	vin.y = vin.y * viewport_matrix[ 3 ] + viewport_matrix[ 1 ];

	*windowx = vin.x;
	*windowy = vin.y;
	*windowz = vin.z;

	return 1;
}

/*!
	Multiply a glm::vec4 by a glm::4x4 matrix.

	\param[in,out] dst A valid glm::vec4 pointer that will be used as the destination variable where the result of the operation will be stored.
	\param[in] v A valid glm::vec4 pointer.
	\param[in] m A valid glm::4x4 matrix pointer.
*/
void Transformation3D::Vec4MultiplyMat4( glm::vec4 *dst, glm::vec4 *v, glm::mat4 *m )
{
	dst->x = ( v->x * (*m)[ 0 ].x ) +
			 ( v->y * (*m)[ 1 ].x ) +
			 ( v->z * (*m)[ 2 ].x ) +
			 ( v->w * (*m)[ 3 ].x );

	dst->y = ( v->x * (*m)[ 0 ].y ) +
			 ( v->y * (*m)[ 1 ].y ) +
			 ( v->z * (*m)[ 2 ].y ) +
			 ( v->w * (*m)[ 3 ].y );

	dst->z = ( v->x * (*m)[ 0 ].z ) +
			 ( v->y * (*m)[ 1 ].z ) +
			 ( v->z * (*m)[ 2 ].z ) +
			 ( v->w * (*m)[ 3 ].z );

	dst->w = ( v->x * (*m)[ 0 ].w ) +
			 ( v->y * (*m)[ 1 ].w ) +
			 ( v->z * (*m)[ 2 ].w ) +
			 ( v->w * (*m)[ 3 ].w );
}

/*!
	Map window coordinates to object coordinates.

	\param[in] windowx Specify the window X coordinate.
	\param[in] windowy Specify the window Y coordinate.
	\param[in] windowz Specify the window Z coordinate.
	\param[in] modelview_matrix Specifies the current modelview matrix.
	\param[in] projection_matrix Specifies the current projection matrix.
	\param[in] viewport_matrix Specifies the current viewport matrix.
	\param[in] objextx Return the computed X object coordinate.
	\param[in] objecty Return the computed Y object coordinate.
	\param[in] objectz Return the computed Z object coordinate.

*/
int Transformation3D::TransformUnproject( float windowx, float windowy, float windowz, glm::mat4 *modelview_matrix, glm::mat4 *projection_matrix, int *viewport_matrix, float *objextx, float *objecty, float *objectz )
{
	glm::mat4 final;

	glm::vec4 vin,
		 vout;

	final = *projection_matrix**modelview_matrix;

	final = glm::inverse(final);
	vin.x = windowx;
	vin.y = windowy;
	vin.z = windowz;
	vin.w = 1.0f;

	vin.x = ( vin.x - viewport_matrix[ 0 ] ) / viewport_matrix[ 2 ];
	vin.y = ( vin.y - viewport_matrix[ 1 ] ) / viewport_matrix[ 3 ];

	vin.x = vin.x * 2.0f - 1.0f;
	vin.y = vin.y * 2.0f - 1.0f;
	vin.z = vin.z * 2.0f - 1.0f;

	Vec4MultiplyMat4( &vout, &vin, &final );

	if( !vout.w ) return 0;

	vout.x /= vout.w;
	vout.y /= vout.w;
	vout.z /= vout.w;

	*objextx = vout.x;
	*objecty = vout.y;
	*objectz = vout.z;

	return 1;
}
