/* Current Working on:
	1. <Done>StringData abstraction; 
	2. TextField and TextContainer and TextContainer definition and how are interface elements arranged
	3. What should Canvas Nodes contain: make use of(e.g. embed) existing 2D Interface elements, or devise new types?
		Node-Property Sync, and What Properties should we expose, and do we ever need a multi-line textfield? Even tab doesn't seem necessary.
		If that is the case, the dimension of IE can be determined at initializatio time.
	4. Approach from Cavans space, and progressively exploiting all implementation details
		- From Loading And Data Creation and Dynamic OpenGL obejct management perspective
		- CanvasNodes oejct definition
	5. Application Usage and Working flow
	6. <Done>Canvas Node Interaction with Property Panel, and More detailed function implementation
	7. Detailed Implementation starting from GraphManager
		- <Done>ShaderResource
		- <Done>FontManager
		- <Done>Interface Objects
			- Property Panels General Implementation
		- <In Progress>Interface Elements
			- Robust and complete implementation
		- <Done, Pending Test>Interface Layout
			- Layout test and implementation
		- <In Progress> Canvs and Canvas Nodes
			- MeshData and MeshResource Detailed Implementation
			- Canvas Logic and Interaction
			- Canvas Nodes
		- Order: 3D Renderable->Canvas Nodes->Collision Detection->Canvas
			- ImageData and Image Resource
			- TextNode, 3DRenderable, and Canvas Interaction
		- Approach From Debugging and Functioning Test POint of View because Canvas interface is a little bit involved, 
		if we could do it while debugging it would be more interesting and intuitive
	8. Debug with 2D Interface
		- <Current Working On> Keyboard input on TextFields on MainMenu and TextContainer in Property Panel{Pending 2 Days Finish}
		- Then we can immediately go to deal with Canvas space 3D renderables
*/

/* Design Decisions:
	1. Landscape camera is obviously more stylish, but a request for first person 
	camera can be heard when thinking about dynamic scene elements. Of course we 
	should avoid our attention on that for now.
	2. Internationalization if Interface Language Or not? Sure.
	3. Images are embeded, but meshes(including mesh spec and mesh data and texture data) are freely editable, even if they might occupy a lot 
	more scenery space than texts and iamges, which makes them seemly more important for portablity and security. 
	This is because I want maximum freedom with offline processing with mesh files. And they are too big to embed. And I just feels like, even
	though meshes are important decoration elements in the scenery, informational data like images and texts are really fundamentally more of 
	real story. And those are what I want to keep safe and compact. Intact and portable.
	4. To truly support streaming, in loading time and CPU memory usage, we MUST dynamically load meshes instead of load all of them at a time
	when application runs. Because when available resources grow, this will impose a huge problem.
	But to do that would require we change: how MeshResource are managed and accessed, how MeshNodes handle their WakeUp function, and how 
	MeshCreationPopupMenu access mesh information through MeshResource object.
*/

/* Technical Notes:
	1. May want a seperate thread for object distance sorting and openGL, 
	collision object creation and deletion; 	To facilitate (camera movement) 
	collisoin detection in main thread.
	2. Window Spaces We Are using: 
		- Window Space: Upper-Left Window Corner Origin, same as GLFW window space, X Y are both positive
		- OpenGL Viewport Sapce: Lower-Left Window Corner Origin, X Y are both positive
		- Viewport Space OpenGL Second Quadrant: Upper-Left Viewport Corner Origin, X Positive Y Negative (Also notice that all the "second quadrant" mentioned in the project actually means forth quadrant....)
	3. Mesh Loading and Streaming:
		- MeshData are used by: MeshResource, MeshCreationPopupMenu, and MeshNode
		Document Loading: When document is loaded, it will first load all specified MeshData information in the map file, then search the two locations for additional MeshData(mesh specs)
		Mesh Adding: In application run-time, user might drag and drop mesh spec files from places other than the two locations, or newly-created meshes in the two locations
		Mesh Lifetime: When MeshNode acquire or unuse MeshData, its reference count will change, but as long as the application is running, MeshResource will have a constant number of loaded mesh datas since they will only increase
		Document Saving: When document is saved, MeshResource will do the ordering of used meshData(i.e. referene count over 0) and save them into the file, others are not saved
		Mesh Node Displaying: MeshData normally only keep those information from the mesh spec file, while other openGL resources are loaded when needed(determined by Canvas), except icon texture(if any). This including: openGL buffer and texture resources
	4. Potentially Useful: 
		https://msdn.microsoft.com/en-us/library/windows/desktop/hh127427%28v=vs.85%29.aspx
	5. Coordinates Convention: 
		DevIL load things from Up to Down, Left to Right, which, when feed into openGL, will be Upside-Down, so when use texture coordinates, we just need to use as if we are counting from Up to down, left to right
	6. When something isn't showing up: only either of both of the following could be the reason: generated vertex, viewProjection matrix, viewport setting, location(model matrix)(e.g. in our case make sure 2D interface obejct rendered in range +X and -Y)
	And for 3D renderable debugging, using [-1,1] Primitive to check for viewport, view, perspect checking will always work -> And either view, model, project matrix, or vertex data is wrong. Also, we have done it many times to have unexpected Draw commands due to copy and paste: e.g. accidental offset, wrong number of vertices specified
	Also Caution the mistake caused by using explicit type conversion without using paranthis properly, or using pointer without using parathesis properly can cause error because of unexpected operator precedence -> Or actually the error because of Z range specification
	7. For 3D TextField mouse collision detecion, one way to do that, if given fixed Node Rotatation and Landscape navigation, or just make sure node always face camera, then we will be able to convert this 3D problem to a 2D one simply by making use of glm::perspective matrix, and fetch the resulted normalized vertex positions.
	However, to make things more generic, and of course landscape navigation isn't the only one we will deploy, we will use ray collision model.
*/

/* Programming Design Specifications<Render Engine>
	0. All renderables are generated in such a way that, their Object space begins at (0,0,0) and extends to (+x, -y, -z)
	1. 3D Renderables use a coodinate system such that, the generated mesh begins at (0,0) object space and extends to [0,+INF) in X and [0, -INF] in Y; The width and height parameters in the canvas node obejct should be the final render display of the canvas node.
*/

/* Debug Convention:
	1. Temporaty Debug Commented expressions are led using "Debug_Temp"
	2. Long-term Debug Sections are commented using "__Debug__"( "or _Debu_" for short), those need to be commented during release
	3. Pending Implementation/Debug code segments using "__Pending"; Use this to concentrate on other parts of the program
*/

/* Naming Convention_Appendix
	1. static variables begin with upper case letter to stress its usage as something like a constant, just like we did the same with enums.
*/

/* Debug/Optimization Considerations
	1. Image And Mesh Data parallel Loading: Caution Library function thread-safety, if not safe, consider using a stack-like loading mechanism: spawn only one thread
	2. Combine String, Image as single 2D/3D renderable type, if necessary
*/

/* Concept References[NOt Throughly Studied]
	http://stackoverflow.com/questions/27382036/zero-initialize-array-member-in-initialization-list
*/

// Cannot seem to find but we are using Left-Upper corner both positive image coor coordinates convention
// Strictly Reinforced: Strictly Avoid Copy code or compare and copy code


//---------------------------------References
// Some Information on Join and Detach a thread:
// http://stackoverflow.com/questions/22426132/do-i-need-to-join-every-thread-in-my-application
// http://www.cplusplus.com/reference/thread/thread/join/
// http://www.cplusplus.com/reference/thread/thread/detach/?kw=detach
// http://www.cplusplus.com/reference/thread/thread/