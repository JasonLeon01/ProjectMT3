---@meta

sf = sf or {}

---@class sf.Drawable
sf.Drawable = sf.Drawable or {}

---@class sf.LuaDrawable : sf.Drawable
sf.LuaDrawable = sf.LuaDrawable or {}
---@type fun(drawCallback: fun(target: sf.RenderTarget, states: sf.RenderStates)): sf.LuaDrawable
sf.LuaDrawable.new = function() end

---@class sf.WindowHandle
sf.WindowHandle = sf.WindowHandle or {}
---@overload fun(): sf.WindowHandle
---@param value integer
---@return sf.WindowHandle
function sf.WindowHandle.new(value) end
---@type fun(value: integer): sf.WindowHandle
sf.WindowHandle.fromInteger = function() end
---@type fun(self: sf.WindowHandle): integer
sf.WindowHandle.toInteger = function() end
--- @brief Represents an angle value.
---@class sf.Angle
sf.Angle = sf.Angle or {}
--- @brief Default constructor
---
--- Sets the angle value to zero.
---@type fun(): sf.Angle
sf.Angle.new = function() end
--- @brief Return the angle's value in degrees
---
--- @return Angle in degrees
---
--- @see `asRadians`
---@type fun(self: sf.Angle): number
sf.Angle.asDegrees = function() end
--- @brief Return the angle's value in radians
---
--- @return Angle in radians
---
--- @see `asDegrees`
---@type fun(self: sf.Angle): number
sf.Angle.asRadians = function() end
--- @brief Wrap to a range such that -180° <= angle < 180°
---
--- Similar to a modulo operation, this returns a copy of the angle
--- constrained to the range [-180°, 180°) == [-Pi, Pi).
--- The resulting angle represents a rotation which is equivalent to `*this`.
---
--- The name "signed" originates from the similarity to signed integers:
--- <table>
--- <tr>
--- <th></th>
--- <th>signed</th>
--- <th>unsigned</th>
--- </tr>
--- <tr>
--- <td>char</td>
--- <td>[-128, 128)</td>
--- <td>[0, 256)</td>
--- </tr>
--- <tr>
--- <td>Angle</td>
--- <td>[-180°, 180°)</td>
--- <td>[0°, 360°)</td>
--- </tr>
--- </table>
---
--- @return Signed angle, wrapped to [-180°, 180°)
---
--- @see `wrapUnsigned`
---@type fun(self: sf.Angle): sf.Angle
sf.Angle.wrapSigned = function() end
--- @brief Wrap to a range such that 0° <= angle < 360°
---
--- Similar to a modulo operation, this returns a copy of the angle
--- constrained to the range [0°, 360°) == [0, Tau) == [0, 2*Pi).
--- The resulting angle represents a rotation which is equivalent to `*this`.
---
--- The name "unsigned" originates from the similarity to unsigned integers:
--- <table>
--- <tr>
--- <th></th>
--- <th>signed</th>
--- <th>unsigned</th>
--- </tr>
--- <tr>
--- <td>char</td>
--- <td>[-128, 128)</td>
--- <td>[0, 256)</td>
--- </tr>
--- <tr>
--- <td>Angle</td>
--- <td>[-180°, 180°)</td>
--- <td>[0°, 360°)</td>
--- </tr>
--- </table>
---
--- @return Unsigned angle, wrapped to [0°, 360°)
---
--- @see `wrapSigned`
---@type fun(self: sf.Angle): sf.Angle
sf.Angle.wrapUnsigned = function() end
--- Predefined 0 degree angle value
---@type sf.Angle
sf.Angle.Zero = nil
--- @brief Construct an angle value from a number of degrees
---
--- @param angle Number of degrees
---
--- @return Angle value constructed from the number of degrees
---
--- @see `radians`
---@type fun(angle: number): sf.Angle
sf.degrees = function() end
--- @brief Construct an angle value from a number of radians
---
--- @param angle Number of radians
---
--- @return Angle value constructed from the number of radians
---
--- @see `degrees`
---@type fun(angle: number): sf.Angle
sf.radians = function() end
--- @brief Abstract class for custom file input streams
---@class sf.InputStream
sf.InputStream = sf.InputStream or {}
--- @brief Read data from the stream
---
--- After reading, the stream's reading position must be
--- advanced by the amount of bytes read.
---
--- @param data Buffer where to copy the read data
--- @param size Desired number of bytes to read
---
--- @return The number of bytes actually read, or `std::nullopt` on error
---@type fun(self: sf.InputStream, size: integer): integer|nil, any
sf.InputStream.read = function() end
--- @brief Change the current reading position
---
--- @param position The position to seek to, from the beginning
---
--- @return The position actually sought to, or `std::nullopt` on error
---@type fun(self: sf.InputStream, position: integer): integer|nil
sf.InputStream.seek = function() end
--- @brief Get the current reading position in the stream
---
--- @return The current position, or `std::nullopt` on error.
---@type fun(self: sf.InputStream): integer|nil
sf.InputStream.tell = function() end
--- @brief Return the size of the stream
---
--- @return The total number of bytes available in the stream, or `std::nullopt` on error
---@type fun(self: sf.InputStream): integer|nil
sf.InputStream.getSize = function() end
--- @brief Implementation of input stream based on a file
---@class sf.FileInputStream : sf.InputStream
sf.FileInputStream = sf.FileInputStream or {}
--- @brief Construct the stream from a file path
---
--- @param filename Name of the file to open
---
--- @throws sf::Exception on error
---@overload fun(): sf.FileInputStream
---@param filename string
---@return sf.FileInputStream
function sf.FileInputStream.new(filename) end
--- @brief Read data from the stream
---
--- After reading, the stream's reading position must be
--- advanced by the amount of bytes read.
---
--- @param data Buffer where to copy the read data
--- @param size Desired number of bytes to read
---
--- @return The number of bytes actually read, or `std::nullopt` on error
---@type fun(self: sf.FileInputStream, size: integer): integer|nil, any
sf.FileInputStream.read = function() end
--- @brief Change the current reading position
---
--- @param position The position to seek to, from the beginning
---
--- @return The position actually sought to, or `std::nullopt` on error
---@type fun(self: sf.FileInputStream, position: integer): integer|nil
sf.FileInputStream.seek = function() end
--- @brief Get the current reading position in the stream
---
--- @return The current position, or `std::nullopt` on error.
---@type fun(self: sf.FileInputStream): integer|nil
sf.FileInputStream.tell = function() end
--- @brief Return the size of the stream
---
--- @return The total number of bytes available in the stream, or `std::nullopt` on error
---@type fun(self: sf.FileInputStream): integer|nil
sf.FileInputStream.getSize = function() end
--- @brief Open the stream from a file path
---
--- On OpenHarmony/HarmonyOS, a path beginning with `rawfile:/`
--- explicitly addresses the HAP rawfile directory. A relative path is
--- first opened from the application filesystem and then, if that fails,
--- from rawfile. `SFML::Main` initializes the native resource manager
--- before application code starts.
--- On Android, paths are first opened from the application filesystem.
--- Relative paths fall back to the packaged asset directory when no
--- filesystem file exists.
---
--- @param filename Name of the file to open
---
--- @return `true` on success, `false` on error
---@type fun(self: sf.FileInputStream, filename: string): boolean
sf.FileInputStream.open = function() end
--- @brief Implementation of input stream based on a memory chunk
---@class sf.MemoryInputStream : sf.InputStream
sf.MemoryInputStream = sf.MemoryInputStream or {}
--- @brief Construct the stream from its data
---
--- @param data        Pointer to the data in memory
--- @param sizeInBytes Size of the data, in bytes
---@type fun(data: any): sf.MemoryInputStream
sf.MemoryInputStream.new = function() end
--- @brief Read data from the stream
---
--- After reading, the stream's reading position must be
--- advanced by the amount of bytes read.
---
--- @param data Buffer where to copy the read data
--- @param size Desired number of bytes to read
---
--- @return The number of bytes actually read, or `std::nullopt` on error
---@type fun(self: sf.MemoryInputStream, size: integer): integer|nil, any
sf.MemoryInputStream.read = function() end
--- @brief Change the current reading position
---
--- @param position The position to seek to, from the beginning
---
--- @return The position actually sought to, or `std::nullopt` on error
---@type fun(self: sf.MemoryInputStream, position: integer): integer|nil
sf.MemoryInputStream.seek = function() end
--- @brief Get the current reading position in the stream
---
--- @return The current position, or `std::nullopt` on error.
---@type fun(self: sf.MemoryInputStream): integer|nil
sf.MemoryInputStream.tell = function() end
--- @brief Return the size of the stream
---
--- @return The total number of bytes available in the stream, or `std::nullopt` on error
---@type fun(self: sf.MemoryInputStream): integer|nil
sf.MemoryInputStream.getSize = function() end
--- @brief Represents a time value
---@class sf.Time
sf.Time = sf.Time or {}
--- @brief Default constructor
---
--- Sets the time value to zero.
---@type fun(): sf.Time
sf.Time.new = function() end
--- @brief Return the time value as a number of seconds
---
--- @return Time in seconds
---
--- @see `asMilliseconds`, `asMicroseconds`
---@type fun(self: sf.Time): number
sf.Time.asSeconds = function() end
--- @brief Return the time value as a number of milliseconds
---
--- @return Time in milliseconds
---
--- @see `asSeconds`, `asMicroseconds`
---@type fun(self: sf.Time): integer
sf.Time.asMilliseconds = function() end
--- @brief Return the time value as a number of microseconds
---
--- @return Time in microseconds
---
--- @see `asSeconds`, `asMilliseconds`
---@type fun(self: sf.Time): integer
sf.Time.asMicroseconds = function() end
--- @brief Return the time value as a `std::chrono::duration`
---
--- @return Time in microseconds
---@type fun(self: sf.Time): any
sf.Time.toDuration = function() end
--- Predefined "zero" time value
---@type sf.Time
sf.Time.Zero = nil
--- @relates Time
--- @brief Construct a time value from a number of seconds
---
--- @param amount Number of seconds
---
--- @return Time value constructed from the amount of seconds
---
--- @see `milliseconds`, `microseconds`
---@type fun(amount: number): sf.Time
sf.seconds = function() end
--- @relates Time
--- @brief Construct a time value from a number of milliseconds
---
--- @param amount Number of milliseconds
---
--- @return Time value constructed from the amount of milliseconds
---
--- @see `seconds`, `microseconds`
---@type fun(amount: integer): sf.Time
sf.milliseconds = function() end
--- @relates Time
--- @brief Construct a time value from a number of microseconds
---
--- @param amount Number of microseconds
---
--- @return Time value constructed from the amount of microseconds
---
--- @see `seconds`, `milliseconds`
---@type fun(amount: integer): sf.Time
sf.microseconds = function() end
--- @brief Utility class that measures the elapsed time
---
--- The clock starts automatically after being constructed.
---@class sf.Clock
sf.Clock = sf.Clock or {}
---@type fun(): sf.Clock
sf.Clock.new = function() end
--- @brief Get the elapsed time
---
--- This function returns the time elapsed since the last call
--- to `restart()` (or the construction of the instance if `restart()`
--- has not been called).
---
--- @return Time elapsed
---@type fun(self: sf.Clock): sf.Time
sf.Clock.getElapsedTime = function() end
--- @brief Check whether the clock is running
---
--- @return `true` if the clock is running, `false` otherwise
---@type fun(self: sf.Clock): boolean
sf.Clock.isRunning = function() end
--- @brief Start the clock
---
--- @see `stop`
---@type fun(self: sf.Clock)
sf.Clock.start = function() end
--- @brief Stop the clock
---
--- @see `start`
---@type fun(self: sf.Clock)
sf.Clock.stop = function() end
--- @brief Restart the clock
---
--- This function puts the time counter back to zero, returns
--- the elapsed time, and leaves the clock in a running state.
---
--- @return Time elapsed
---
--- @see `reset`
---@type fun(self: sf.Clock): sf.Time
sf.Clock.restart = function() end
--- @brief Reset the clock
---
--- This function puts the time counter back to zero, returns
--- the elapsed time, and leaves the clock in a paused state.
---
--- @return Time elapsed
---
--- @see `restart`
---@type fun(self: sf.Clock): sf.Time
sf.Clock.reset = function() end
--- @ingroup system
--- @brief Make the current thread sleep for a given duration
---
--- `sf::sleep` is the best way to block a program or one of its
--- threads, as it doesn't consume any CPU power. Compared to
--- the standard `std::this_thread::sleep_for` function, this
--- one provides more accurate sleeping time thanks to some
--- platform-specific tweaks.
---
--- `sf::sleep` only guarantees millisecond precision. Sleeping
--- for a duration less than 1 millisecond is prone to result
--- in the actual sleep duration being less than what is
--- requested.
---
--- @param duration Time to sleep
---@type fun(duration: sf.Time)
sf.sleep = function() end
--- @brief Utility class providing hybrid functionality
--- of a timeout and a continuation predicate
---@class sf.TimeoutWithPredicate
sf.TimeoutWithPredicate = sf.TimeoutWithPredicate or {}
--- @brief Constructor
---
--- This constructor constructs a `TimeoutWithPredicate` object
--- that times out after the given amount of time.
---
--- @param timeout Time to timeout after
---@overload fun(predicate: fun(): boolean, period: sf.Time): sf.TimeoutWithPredicate
---@overload fun(predicate: fun(): boolean): sf.TimeoutWithPredicate
---@param timeout sf.Time
---@return sf.TimeoutWithPredicate
function sf.TimeoutWithPredicate.new(timeout) end
--- @brief Get the period
---
--- @return The period
---@type fun(self: sf.TimeoutWithPredicate): sf.Time
sf.TimeoutWithPredicate.getPeriod = function() end
--- @brief Class template for manipulating
--- 2-dimensional vectors
---@class sf.Vector2i
--- X coordinate of the vector
---@field x integer
--- Y coordinate of the vector
---@field y integer
sf.Vector2i = sf.Vector2i or {}
--- @brief Construct the vector from cartesian coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
---@overload fun(): sf.Vector2i
---@param x integer
---@param y integer
---@return sf.Vector2i
function sf.Vector2i.new(x, y) end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector2i): integer
sf.Vector2i.lengthSquared = function() end
--- @brief Returns a perpendicular vector.
---
--- Returns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).
--- For example, the vector (1,0) is transformed to (0,1).
---
--- In SFML's default coordinate system with +X right and +Y down,
--- this amounts to a clockwise rotation.
---@type fun(self: sf.Vector2i): sf.Vector2i
sf.Vector2i.perpendicular = function() end
--- @brief Dot product of two 2D vectors.
---@type fun(self: sf.Vector2i, rhs: sf.Vector2i): integer
sf.Vector2i.dot = function() end
--- @brief Z component of the cross product of two 2D vectors.
---
--- Treats the operands as 3D vectors, computes their cross product
--- and returns the result's Z component (X and Y components are always zero).
---@type fun(self: sf.Vector2i, rhs: sf.Vector2i): integer
sf.Vector2i.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i
sf.Vector2i.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector2i, rhs: sf.Vector2i): sf.Vector2i
sf.Vector2i.componentWiseDiv = function() end
---@type fun(self: sf.Vector2i): integer, integer
sf.Vector2i.unpack = function() end

---@class sf.Vector2i
---@operator unm: sf.Vector2i

---@class sf.Vector2i
---@operator add(sf.Vector2i): sf.Vector2i

---@class sf.Vector2i
---@operator sub(sf.Vector2i): sf.Vector2i

---@class sf.Vector2i
---@operator mul(integer): sf.Vector2i

---@class sf.Vector2i
---@operator div(integer): sf.Vector2i

---@class sf.Vector2i
---@operator eq(sf.Vector2i): boolean
--- @brief Class template for manipulating
--- 2-dimensional vectors
---@class sf.Vector2u
--- X coordinate of the vector
---@field x integer
--- Y coordinate of the vector
---@field y integer
sf.Vector2u = sf.Vector2u or {}
--- @brief Construct the vector from cartesian coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
---@overload fun(): sf.Vector2u
---@param x integer
---@param y integer
---@return sf.Vector2u
function sf.Vector2u.new(x, y) end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector2u): integer
sf.Vector2u.lengthSquared = function() end
--- @brief Returns a perpendicular vector.
---
--- Returns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).
--- For example, the vector (1,0) is transformed to (0,1).
---
--- In SFML's default coordinate system with +X right and +Y down,
--- this amounts to a clockwise rotation.
---@type fun(self: sf.Vector2u): sf.Vector2u
sf.Vector2u.perpendicular = function() end
--- @brief Dot product of two 2D vectors.
---@type fun(self: sf.Vector2u, rhs: sf.Vector2u): integer
sf.Vector2u.dot = function() end
--- @brief Z component of the cross product of two 2D vectors.
---
--- Treats the operands as 3D vectors, computes their cross product
--- and returns the result's Z component (X and Y components are always zero).
---@type fun(self: sf.Vector2u, rhs: sf.Vector2u): integer
sf.Vector2u.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u
sf.Vector2u.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector2u, rhs: sf.Vector2u): sf.Vector2u
sf.Vector2u.componentWiseDiv = function() end
---@type fun(self: sf.Vector2u): integer, integer
sf.Vector2u.unpack = function() end

---@class sf.Vector2u
---@operator unm: sf.Vector2u

---@class sf.Vector2u
---@operator add(sf.Vector2u): sf.Vector2u

---@class sf.Vector2u
---@operator sub(sf.Vector2u): sf.Vector2u

---@class sf.Vector2u
---@operator mul(integer): sf.Vector2u

---@class sf.Vector2u
---@operator div(integer): sf.Vector2u

---@class sf.Vector2u
---@operator eq(sf.Vector2u): boolean
--- @brief Class template for manipulating
--- 2-dimensional vectors
---@class sf.Vector2f
--- X coordinate of the vector
---@field x number
--- Y coordinate of the vector
---@field y number
sf.Vector2f = sf.Vector2f or {}
--- @brief Construct the vector from cartesian coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
---@overload fun(r: number, phi: sf.Angle): sf.Vector2f
---@overload fun(): sf.Vector2f
---@param x number
---@param y number
---@return sf.Vector2f
function sf.Vector2f.new(x, y) end
--- @brief Length of the vector <i><b>(floating-point)</b></i>.
---
--- If you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.
---@type fun(self: sf.Vector2f): number
sf.Vector2f.length = function() end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector2f): number
sf.Vector2f.lengthSquared = function() end
--- @brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.
---
--- \pre `*this` is no zero vector.
---@type fun(self: sf.Vector2f): sf.Vector2f
sf.Vector2f.normalized = function() end
--- @brief Signed angle from `*this` to `rhs` <i><b>(floating-point)</b></i>.
---
--- @return The smallest angle which rotates `*this` in positive
--- or negative direction, until it has the same direction as `rhs`.
--- The result has a sign and lies in the range [-180, 180) degrees.
--- \pre Neither `*this` nor `rhs` is a zero vector.
---@type fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Angle
sf.Vector2f.angleTo = function() end
--- @brief Signed angle from +X or (1,0) vector <i><b>(floating-point)</b></i>.
---
--- For example, the vector (1,0) corresponds to 0 degrees, (0,1) corresponds to 90 degrees.
---
--- @return Angle in the range [-180, 180) degrees.
--- \pre This vector is no zero vector.
---@type fun(self: sf.Vector2f): sf.Angle
sf.Vector2f.angle = function() end
--- @brief Rotate by angle @c phi <i><b>(floating-point)</b></i>.
---
--- Returns a vector with same length but different direction.
---
--- In SFML's default coordinate system with +X right and +Y down,
--- this amounts to a clockwise rotation by `phi`.
---@type fun(self: sf.Vector2f, phi: sf.Angle): sf.Vector2f
sf.Vector2f.rotatedBy = function() end
--- @brief Projection of this vector onto `axis` <i><b>(floating-point)</b></i>.
---
--- @param axis Vector being projected onto. Need not be normalized.
--- \pre `axis` must not have length zero.
---@type fun(self: sf.Vector2f, axis: sf.Vector2f): sf.Vector2f
sf.Vector2f.projectedOnto = function() end
--- @brief Returns a perpendicular vector.
---
--- Returns `*this` rotated by +90 degrees; (x,y) becomes (-y,x).
--- For example, the vector (1,0) is transformed to (0,1).
---
--- In SFML's default coordinate system with +X right and +Y down,
--- this amounts to a clockwise rotation.
---@type fun(self: sf.Vector2f): sf.Vector2f
sf.Vector2f.perpendicular = function() end
--- @brief Dot product of two 2D vectors.
---@type fun(self: sf.Vector2f, rhs: sf.Vector2f): number
sf.Vector2f.dot = function() end
--- @brief Z component of the cross product of two 2D vectors.
---
--- Treats the operands as 3D vectors, computes their cross product
--- and returns the result's Z component (X and Y components are always zero).
---@type fun(self: sf.Vector2f, rhs: sf.Vector2f): number
sf.Vector2f.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f
sf.Vector2f.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector2f, rhs: sf.Vector2f): sf.Vector2f
sf.Vector2f.componentWiseDiv = function() end
---@type fun(self: sf.Vector2f): number, number
sf.Vector2f.unpack = function() end

---@class sf.Vector2f
---@operator unm: sf.Vector2f

---@class sf.Vector2f
---@operator add(sf.Vector2f): sf.Vector2f

---@class sf.Vector2f
---@operator sub(sf.Vector2f): sf.Vector2f

---@class sf.Vector2f
---@operator mul(number): sf.Vector2f

---@class sf.Vector2f
---@operator div(number): sf.Vector2f

---@class sf.Vector2f
---@operator eq(sf.Vector2f): boolean
--- @brief Utility template class for manipulating
--- 3-dimensional vectors
---@class sf.Vector3i
--- X coordinate of the vector
---@field x integer
--- Y coordinate of the vector
---@field y integer
--- Z coordinate of the vector
---@field z integer
sf.Vector3i = sf.Vector3i or {}
--- @brief Construct the vector from its coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
--- @param z Z coordinate
---@overload fun(): sf.Vector3i
---@param x integer
---@param y integer
---@param z integer
---@return sf.Vector3i
function sf.Vector3i.new(x, y, z) end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector3i): integer
sf.Vector3i.lengthSquared = function() end
--- @brief Dot product of two 3D vectors.
---@type fun(self: sf.Vector3i, rhs: sf.Vector3i): integer
sf.Vector3i.dot = function() end
--- @brief Cross product of two 3D vectors.
---@type fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i
sf.Vector3i.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i
sf.Vector3i.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector3i, rhs: sf.Vector3i): sf.Vector3i
sf.Vector3i.componentWiseDiv = function() end
---@type fun(self: sf.Vector3i): integer, integer, integer
sf.Vector3i.unpack = function() end

---@class sf.Vector3i
---@operator unm: sf.Vector3i

---@class sf.Vector3i
---@operator add(sf.Vector3i): sf.Vector3i

---@class sf.Vector3i
---@operator sub(sf.Vector3i): sf.Vector3i

---@class sf.Vector3i
---@operator mul(integer): sf.Vector3i

---@class sf.Vector3i
---@operator div(integer): sf.Vector3i

---@class sf.Vector3i
---@operator eq(sf.Vector3i): boolean
--- @brief Utility template class for manipulating
--- 3-dimensional vectors
---@class sf.Vector3u
--- X coordinate of the vector
---@field x integer
--- Y coordinate of the vector
---@field y integer
--- Z coordinate of the vector
---@field z integer
sf.Vector3u = sf.Vector3u or {}
--- @brief Construct the vector from its coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
--- @param z Z coordinate
---@overload fun(): sf.Vector3u
---@param x integer
---@param y integer
---@param z integer
---@return sf.Vector3u
function sf.Vector3u.new(x, y, z) end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector3u): integer
sf.Vector3u.lengthSquared = function() end
--- @brief Dot product of two 3D vectors.
---@type fun(self: sf.Vector3u, rhs: sf.Vector3u): integer
sf.Vector3u.dot = function() end
--- @brief Cross product of two 3D vectors.
---@type fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u
sf.Vector3u.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u
sf.Vector3u.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector3u, rhs: sf.Vector3u): sf.Vector3u
sf.Vector3u.componentWiseDiv = function() end
---@type fun(self: sf.Vector3u): integer, integer, integer
sf.Vector3u.unpack = function() end

---@class sf.Vector3u
---@operator unm: sf.Vector3u

---@class sf.Vector3u
---@operator add(sf.Vector3u): sf.Vector3u

---@class sf.Vector3u
---@operator sub(sf.Vector3u): sf.Vector3u

---@class sf.Vector3u
---@operator mul(integer): sf.Vector3u

---@class sf.Vector3u
---@operator div(integer): sf.Vector3u

---@class sf.Vector3u
---@operator eq(sf.Vector3u): boolean
--- @brief Utility template class for manipulating
--- 3-dimensional vectors
---@class sf.Vector3f
--- X coordinate of the vector
---@field x number
--- Y coordinate of the vector
---@field y number
--- Z coordinate of the vector
---@field z number
sf.Vector3f = sf.Vector3f or {}
--- @brief Construct the vector from its coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
--- @param z Z coordinate
---@overload fun(): sf.Vector3f
---@param x number
---@param y number
---@param z number
---@return sf.Vector3f
function sf.Vector3f.new(x, y, z) end
--- @brief Length of the vector <i><b>(floating-point)</b></i>.
---
--- If you are not interested in the actual length, but only in comparisons, consider using `lengthSquared()`.
---@type fun(self: sf.Vector3f): number
sf.Vector3f.length = function() end
--- @brief Square of vector's length.
---
--- Suitable for comparisons, more efficient than `length()`.
---@type fun(self: sf.Vector3f): number
sf.Vector3f.lengthSquared = function() end
--- @brief Vector with same direction but length 1 <i><b>(floating-point)</b></i>.
---
--- \pre `*this` is no zero vector.
---@type fun(self: sf.Vector3f): sf.Vector3f
sf.Vector3f.normalized = function() end
--- @brief Dot product of two 3D vectors.
---@type fun(self: sf.Vector3f, rhs: sf.Vector3f): number
sf.Vector3f.dot = function() end
--- @brief Cross product of two 3D vectors.
---@type fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f
sf.Vector3f.cross = function() end
--- @brief Component-wise multiplication of `*this` and `rhs`.
---
--- Computes `(lhs.x*rhs.x, lhs.y*rhs.y, lhs.z*rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
--- This operation is also known as the Hadamard or Schur product.
---@type fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f
sf.Vector3f.componentWiseMul = function() end
--- @brief Component-wise division of `*this` and `rhs`.
---
--- Computes `(lhs.x/rhs.x, lhs.y/rhs.y, lhs.z/rhs.z)`.
---
--- Scaling is the most common use case for component-wise multiplication/division.
---
--- \pre Neither component of `rhs` is zero.
---@type fun(self: sf.Vector3f, rhs: sf.Vector3f): sf.Vector3f
sf.Vector3f.componentWiseDiv = function() end
---@type fun(self: sf.Vector3f): number, number, number
sf.Vector3f.unpack = function() end

---@class sf.Vector3f
---@operator unm: sf.Vector3f

---@class sf.Vector3f
---@operator add(sf.Vector3f): sf.Vector3f

---@class sf.Vector3f
---@operator sub(sf.Vector3f): sf.Vector3f

---@class sf.Vector3f
---@operator mul(number): sf.Vector3f

---@class sf.Vector3f
---@operator div(number): sf.Vector3f

---@class sf.Vector3f
---@operator eq(sf.Vector3f): boolean

---@class sf.Version
--- SFML major version
---@field major integer
--- SFML minor version
---@field minor integer
--- SFML patch version
---@field patch integer
--- `true` if this is a release version, `false` if this is a development version
---@field isRelease boolean
--- String representation of the SFML version, e.g. 3.1.0 or 3.1.0-dev
---@field string string
sf.Version = sf.Version or {}
--- @brief Retrieve the runtime version of the SFML library
---
--- The SFML_VERSION_MAJOR, SFML_VERSION_MINOR,
--- SFML_VERSION_PATCH and SFML_VERSION_IS_RELEASE defines
--- only provide a way to determine the SFML library version
--- at compile time. If an application is dynamically linked
--- to SFML, the version of the library that is loaded at
--- runtime might not be the same as the version of the
--- library headers which the application included when it
--- was built. In order for an application to determine
--- which SFML version is currently loaded at runtime, it
--- can use this function. This function relies on version
--- information embedded into the loaded library when it was
--- built. This information can be useful when an application
--- has to determine the SFML version in order to diagnose
--- problems and be able to report them to the library
--- maintainers.
---
--- @return The version of the SFML library
---@type fun(): sf.Version
sf.version = function() end
sf.Clipboard = sf.Clipboard or {}
--- @brief Get the content of the clipboard as string data
---
--- This function returns the content of the clipboard
--- as a string. If the clipboard does not contain string
--- it returns an empty `sf::String` object.
---
--- @return Clipboard contents as `sf::String` object
---@type fun(): string
sf.Clipboard.getString = function() end
--- @brief Set the content of the clipboard as string data
---
--- This function sets the content of the clipboard as a
--- string.
---
--- @warning Due to limitations on some operating systems,
--- setting the clipboard contents is only
--- guaranteed to work if there is currently an
--- open window for which events are being handled.
---
--- @param text `sf::String` containing the data to be sent
--- to the clipboard
---@type fun(text: string)
sf.Clipboard.setString = function() end
--- @brief Structure defining the settings of the OpenGL
--- context attached to a window
---@class sf.ContextSettings
--- Bits of the depth buffer
---@field depthBits integer
--- Bits of the stencil buffer
---@field stencilBits integer
--- Level of anti-aliasing
---@field antiAliasingLevel integer
--- Major number of the context version to create
---@field majorVersion integer
--- Minor number of the context version to create
---@field minorVersion integer
--- The attribute flags to create the context with
---@field attributeFlags integer
--- Whether the context framebuffer is sRGB capable
---@field sRgbCapable boolean
sf.ContextSettings = sf.ContextSettings or {}
---@type fun(): sf.ContextSettings
sf.ContextSettings.new = function() end
--- @brief Enumeration of the context attribute flags
---@class sf.ContextSettings.Attribute
--- Non-debug, compatibility context (this and the core attribute are mutually exclusive)
---@field Default integer
--- Core attribute
---@field Core integer
--- Debug attribute
---@field Debug integer
sf.ContextSettings.Attribute = sf.ContextSettings.Attribute or {}
---@alias sf.GlFunctionPointer sf.void (*)()
--- @brief Class holding a valid drawing context
---@class sf.Context
sf.Context = sf.Context or {}
--- @brief Construct a in-memory context
---
--- This constructor is for internal use, you don't need
--- to bother with it.
---
--- @param settings Creation parameters
--- @param size     Back buffer size
---@overload fun(): sf.Context
---@param settings sf.ContextSettings
---@param size sf.Vector2u
---@return sf.Context
function sf.Context.new(settings, size) end
--- @brief Activate or deactivate explicitly the context
---
--- @param active `true` to activate, `false` to deactivate
---
--- @return `true` on success, `false` on failure
---@type fun(self: sf.Context, active: boolean): boolean
sf.Context.setActive = function() end
--- @brief Get the settings of the context
---
--- Note that these settings may be different than the ones
--- passed to the constructor; they are indeed adjusted if the
--- original settings are not directly supported by the system.
---
--- @return Structure containing the settings
---@type fun(self: sf.Context): sf.ContextSettings
sf.Context.getSettings = function() end
--- @brief Check whether a given OpenGL extension is available
---
--- @param name Name of the extension to check for
---
--- @return `true` if available, `false` if unavailable
---@type fun(name: string): boolean
sf.Context.isExtensionAvailable = function() end
--- @brief Get the currently active context
---
--- This function will only return `sf::Context` objects.
--- Contexts created e.g. by RenderTargets or for internal
--- use will not be returned by this function.
---
--- @return The currently active context or `nullptr` if none is active
---@type fun(): sf.Context
sf.Context.getActiveContext = function() end
--- @brief Get the currently active context's ID
---
--- The context ID is used to identify contexts when
--- managing unshareable OpenGL resources.
---
--- @return The active context's ID or 0 if no context is currently active
---@type fun(): integer
sf.Context.getActiveContextId = function() end
--- @brief Cursor defines the appearance of a system cursor
---@class sf.Cursor
sf.Cursor = sf.Cursor or {}
--- @brief Create a native system cursor
---
--- Refer to the list of cursor available on each system
--- (see `sf::Cursor::Type`) to know whether a given cursor is
--- expected to load successfully or is not supported by
--- the operating system.
---
--- @param type Native system cursor type
---
--- @throws sf::Exception if the corresponding cursor
--- is not natively supported by the operating
--- system
---@overload fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor
---@param type sf.Cursor.Type
---@return sf.Cursor
function sf.Cursor.new(type) end
--- @brief Create a cursor with the provided image
---
--- `pixels` must be an array of `size` pixels
--- in 32-bit RGBA format. If not, this will cause undefined behavior.
---
--- If `pixels` is `nullptr` or either of `size`'s
--- properties are 0, the current cursor is left unchanged
--- and the function will return `false`.
---
--- In addition to specifying the pixel data, you can also
--- specify the location of the hotspot of the cursor. The
--- hotspot is the pixel coordinate within the cursor image
--- which will be located exactly where the mouse pointer
--- position is. Any mouse actions that are performed will
--- return the window/screen location of the hotspot.
---
--- @warning On Unix platforms which do not support colored
--- cursors, the pixels are mapped into a monochrome
--- bitmap: pixels with an alpha channel to 0 are
--- transparent, black if the RGB channel are close
--- to zero, and white otherwise.
---
--- @param pixels   Array of pixels of the image
--- @param size     Width and height of the image
--- @param hotspot  (x,y) location of the hotspot
--- @return Cursor if the cursor was successfully loaded;
--- `std::nullopt` otherwise
---@type fun(pixels: any, size: sf.Vector2u, hotspot: sf.Vector2u): sf.Cursor|nil
sf.Cursor.createFromPixels = function() end
--- @brief Create a native system cursor
---
--- Refer to the list of cursor available on each system
--- (see `sf::Cursor::Type`) to know whether a given cursor is
--- expected to load successfully or is not supported by
--- the operating system.
---
--- @param type Native system cursor type
--- @return Cursor if and only if the corresponding cursor is
--- natively supported by the operating system;
--- `std::nullopt` otherwise
---@type fun(type: sf.Cursor.Type): sf.Cursor|nil
sf.Cursor.createFromSystem = function() end
--- @brief Enumeration of the native system cursor types
---
--- Refer to the following table to determine which cursor
--- is available on which platform.
---
--- Type                                       | Linux | macOS | Windows  |
--- --------------------------------------------|:-----:|:-----:|:--------:|
--- `sf::Cursor::Type::Arrow`                  |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::ArrowWait`              |  no   | no    |   yes    |
--- `sf::Cursor::Type::Wait`                   |  yes  | no    |   yes    |
--- `sf::Cursor::Type::Text`                   |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::Hand`                   |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::SizeHorizontal`         |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::SizeVertical`           |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::SizeTopLeftBottomRight` |  no   | yes*  |   yes    |
--- `sf::Cursor::Type::SizeBottomLeftTopRight` |  no   | yes*  |   yes    |
--- `sf::Cursor::Type::SizeLeft`               |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeRight`              |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeTop`                |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeBottom`             |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeTopLeft`            |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeTopRight`           |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeBottomLeft`         |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeBottomRight`        |  yes  | yes** |   yes**  |
--- `sf::Cursor::Type::SizeAll`                |  yes  | no    |   yes    |
--- `sf::Cursor::Type::Cross`                  |  yes  | yes   |   yes    |
--- `sf::Cursor::Type::Help`                   |  yes  | yes*  |   yes    |
--- `sf::Cursor::Type::NotAllowed`             |  yes  | yes   |   yes    |
---
--- * These cursor types are undocumented so may not
--- be available on all versions, but have been tested on 10.13
---
--- ** On Windows and macOS, double-headed arrows are used
---@class sf.Cursor.Type
--- Arrow cursor (default)
---@field Arrow sf.Cursor.Type
--- Busy arrow cursor
---@field ArrowWait sf.Cursor.Type
--- Busy cursor
---@field Wait sf.Cursor.Type
--- I-beam, cursor when hovering over a field allowing text entry
---@field Text sf.Cursor.Type
--- Pointing hand cursor
---@field Hand sf.Cursor.Type
--- Horizontal double arrow cursor
---@field SizeHorizontal sf.Cursor.Type
--- Vertical double arrow cursor
---@field SizeVertical sf.Cursor.Type
--- Double arrow cursor going from top-left to bottom-right
---@field SizeTopLeftBottomRight sf.Cursor.Type
--- Double arrow cursor going from bottom-left to top-right
---@field SizeBottomLeftTopRight sf.Cursor.Type
--- Left arrow cursor on Linux, same as SizeHorizontal on other platforms
---@field SizeLeft sf.Cursor.Type
--- Right arrow cursor on Linux, same as SizeHorizontal on other platforms
---@field SizeRight sf.Cursor.Type
--- Up arrow cursor on Linux, same as SizeVertical on other platforms
---@field SizeTop sf.Cursor.Type
--- Down arrow cursor on Linux, same as SizeVertical on other platforms
---@field SizeBottom sf.Cursor.Type
--- Top-left arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms
---@field SizeTopLeft sf.Cursor.Type
--- Bottom-right arrow cursor on Linux, same as SizeTopLeftBottomRight on other platforms
---@field SizeBottomRight sf.Cursor.Type
--- Bottom-left arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms
---@field SizeBottomLeft sf.Cursor.Type
--- Top-right arrow cursor on Linux, same as SizeBottomLeftTopRight on other platforms
---@field SizeTopRight sf.Cursor.Type
--- Combination of SizeHorizontal and SizeVertical
---@field SizeAll sf.Cursor.Type
--- Crosshair cursor
---@field Cross sf.Cursor.Type
--- Help cursor
---@field Help sf.Cursor.Type
--- Action not allowed cursor
---@field NotAllowed sf.Cursor.Type
sf.Cursor.Type = sf.Cursor.Type or {}
sf.Joystick = sf.Joystick or {}
--- Maximum number of supported joysticks
---@type integer
sf.Joystick.Count = nil
--- Maximum number of supported buttons
---@type integer
sf.Joystick.ButtonCount = nil
--- Maximum number of supported axes
---@type integer
sf.Joystick.AxisCount = nil
--- @brief Axes supported by SFML joysticks
---@class sf.Joystick.Axis
--- The X axis
---@field X sf.Joystick.Axis
--- The Y axis
---@field Y sf.Joystick.Axis
--- The Z axis
---@field Z sf.Joystick.Axis
--- The R axis
---@field R sf.Joystick.Axis
--- The U axis
---@field U sf.Joystick.Axis
--- The V axis
---@field V sf.Joystick.Axis
--- The X axis of the point-of-view hat
---@field PovX sf.Joystick.Axis
--- The Y axis of the point-of-view hat
---@field PovY sf.Joystick.Axis
sf.Joystick.Axis = sf.Joystick.Axis or {}
--- @brief Structure holding a joystick's identification
---@class sf.Joystick.Identification
--- Name of the joystick
---@field name string
--- Manufacturer identifier
---@field vendorId integer
--- Product identifier
---@field productId integer
sf.Joystick.Identification = sf.Joystick.Identification or {}
---@type fun(): sf.Joystick.Identification
sf.Joystick.Identification.new = function() end
--- @brief Check if a joystick is connected
---
--- @param joystick Index of the joystick to check
---
--- @return `true` if the joystick is connected, `false` otherwise
---@type fun(joystick: integer): boolean
sf.Joystick.isConnected = function() end
--- @brief Return the number of buttons supported by a joystick
---
--- If the joystick is not connected, this function returns 0.
---
--- @param joystick Index of the joystick
---
--- @return Number of buttons supported by the joystick
---@type fun(joystick: integer): integer
sf.Joystick.getButtonCount = function() end
--- @brief Check if a joystick supports a given axis
---
--- If the joystick is not connected, this function returns `false`.
---
--- @param joystick Index of the joystick
--- @param axis     Axis to check
---
--- @return `true` if the joystick supports the axis, `false` otherwise
---@type fun(joystick: integer, axis: sf.Joystick.Axis): boolean
sf.Joystick.hasAxis = function() end
--- @brief Check if a joystick button is pressed
---
--- If the joystick is not connected, this function returns `false`.
---
--- @param joystick Index of the joystick
--- @param button   Button to check
---
--- @return `true` if the button is pressed, `false` otherwise
---@type fun(joystick: integer, button: integer): boolean
sf.Joystick.isButtonPressed = function() end
--- @brief Get the current position of a joystick axis
---
--- If the joystick is not connected, this function returns 0.
---
--- @param joystick Index of the joystick
--- @param axis     Axis to check
---
--- @return Current position of the axis, in range [-100 .. 100]
---@type fun(joystick: integer, axis: sf.Joystick.Axis): number
sf.Joystick.getAxisPosition = function() end
--- @brief Get the joystick information
---
--- @param joystick Index of the joystick
---
--- @return Structure containing joystick information.
---@type fun(joystick: integer): sf.Joystick.Identification
sf.Joystick.getIdentification = function() end
--- @brief Update the states of all joysticks
---
--- This function is used internally by SFML, so you normally
--- don't have to call it explicitly. However, you may need to
--- call it if you have no window yet (or no window at all):
--- in this case the joystick states are not updated automatically.
---@type fun()
sf.Joystick.update = function() end
--- @brief Key codes
---
--- The enumerators refer to the "localized" key; i.e. depending
--- on the layout set by the operating system, a key can be mapped
--- to `Y` or `Z`.
---@class sf.Keyboard.Key
--- Unhandled key
---@field Unknown sf.Keyboard.Key
--- The A key
---@field A sf.Keyboard.Key
--- The B key
---@field B sf.Keyboard.Key
--- The C key
---@field C sf.Keyboard.Key
--- The D key
---@field D sf.Keyboard.Key
--- The E key
---@field E sf.Keyboard.Key
--- The F key
---@field F sf.Keyboard.Key
--- The G key
---@field G sf.Keyboard.Key
--- The H key
---@field H sf.Keyboard.Key
--- The I key
---@field I sf.Keyboard.Key
--- The J key
---@field J sf.Keyboard.Key
--- The K key
---@field K sf.Keyboard.Key
--- The L key
---@field L sf.Keyboard.Key
--- The M key
---@field M sf.Keyboard.Key
--- The N key
---@field N sf.Keyboard.Key
--- The O key
---@field O sf.Keyboard.Key
--- The P key
---@field P sf.Keyboard.Key
--- The Q key
---@field Q sf.Keyboard.Key
--- The R key
---@field R sf.Keyboard.Key
--- The S key
---@field S sf.Keyboard.Key
--- The T key
---@field T sf.Keyboard.Key
--- The U key
---@field U sf.Keyboard.Key
--- The V key
---@field V sf.Keyboard.Key
--- The W key
---@field W sf.Keyboard.Key
--- The X key
---@field X sf.Keyboard.Key
--- The Y key
---@field Y sf.Keyboard.Key
--- The Z key
---@field Z sf.Keyboard.Key
--- The 0 key
---@field Num0 sf.Keyboard.Key
--- The 1 key
---@field Num1 sf.Keyboard.Key
--- The 2 key
---@field Num2 sf.Keyboard.Key
--- The 3 key
---@field Num3 sf.Keyboard.Key
--- The 4 key
---@field Num4 sf.Keyboard.Key
--- The 5 key
---@field Num5 sf.Keyboard.Key
--- The 6 key
---@field Num6 sf.Keyboard.Key
--- The 7 key
---@field Num7 sf.Keyboard.Key
--- The 8 key
---@field Num8 sf.Keyboard.Key
--- The 9 key
---@field Num9 sf.Keyboard.Key
--- The Escape key
---@field Escape sf.Keyboard.Key
--- The left Control key
---@field LControl sf.Keyboard.Key
--- The left Shift key
---@field LShift sf.Keyboard.Key
--- The left Alt key
---@field LAlt sf.Keyboard.Key
--- The left OS specific key: window (Windows and Linux), apple (macOS), ...
---@field LSystem sf.Keyboard.Key
--- The right Control key
---@field RControl sf.Keyboard.Key
--- The right Shift key
---@field RShift sf.Keyboard.Key
--- The right Alt key
---@field RAlt sf.Keyboard.Key
--- The right OS specific key: window (Windows and Linux), apple (macOS), ...
---@field RSystem sf.Keyboard.Key
--- The Menu key
---@field Menu sf.Keyboard.Key
--- The [ key
---@field LBracket sf.Keyboard.Key
--- The ] key
---@field RBracket sf.Keyboard.Key
--- The ; key
---@field Semicolon sf.Keyboard.Key
--- The , key
---@field Comma sf.Keyboard.Key
--- The . key
---@field Period sf.Keyboard.Key
--- The ' key
---@field Apostrophe sf.Keyboard.Key
--- The / key
---@field Slash sf.Keyboard.Key
--- The \ key
---@field Backslash sf.Keyboard.Key
--- The ` key
---@field Grave sf.Keyboard.Key
--- The = key
---@field Equal sf.Keyboard.Key
--- The - key (hyphen)
---@field Hyphen sf.Keyboard.Key
--- The Space key
---@field Space sf.Keyboard.Key
--- The Enter/Return keys
---@field Enter sf.Keyboard.Key
--- The Backspace key
---@field Backspace sf.Keyboard.Key
--- The Tabulation key
---@field Tab sf.Keyboard.Key
--- The Page up key
---@field PageUp sf.Keyboard.Key
--- The Page down key
---@field PageDown sf.Keyboard.Key
--- The End key
---@field End sf.Keyboard.Key
--- The Home key
---@field Home sf.Keyboard.Key
--- The Insert key
---@field Insert sf.Keyboard.Key
--- The Delete key
---@field Delete sf.Keyboard.Key
--- The + key
---@field Add sf.Keyboard.Key
--- The - key (minus, usually from numpad)
---@field Subtract sf.Keyboard.Key
--- The * key
---@field Multiply sf.Keyboard.Key
--- The / key
---@field Divide sf.Keyboard.Key
--- Left arrow
---@field Left sf.Keyboard.Key
--- Right arrow
---@field Right sf.Keyboard.Key
--- Up arrow
---@field Up sf.Keyboard.Key
--- Down arrow
---@field Down sf.Keyboard.Key
--- The numpad 0 key
---@field Numpad0 sf.Keyboard.Key
--- The numpad 1 key
---@field Numpad1 sf.Keyboard.Key
--- The numpad 2 key
---@field Numpad2 sf.Keyboard.Key
--- The numpad 3 key
---@field Numpad3 sf.Keyboard.Key
--- The numpad 4 key
---@field Numpad4 sf.Keyboard.Key
--- The numpad 5 key
---@field Numpad5 sf.Keyboard.Key
--- The numpad 6 key
---@field Numpad6 sf.Keyboard.Key
--- The numpad 7 key
---@field Numpad7 sf.Keyboard.Key
--- The numpad 8 key
---@field Numpad8 sf.Keyboard.Key
--- The numpad 9 key
---@field Numpad9 sf.Keyboard.Key
--- The F1 key
---@field F1 sf.Keyboard.Key
--- The F2 key
---@field F2 sf.Keyboard.Key
--- The F3 key
---@field F3 sf.Keyboard.Key
--- The F4 key
---@field F4 sf.Keyboard.Key
--- The F5 key
---@field F5 sf.Keyboard.Key
--- The F6 key
---@field F6 sf.Keyboard.Key
--- The F7 key
---@field F7 sf.Keyboard.Key
--- The F8 key
---@field F8 sf.Keyboard.Key
--- The F9 key
---@field F9 sf.Keyboard.Key
--- The F10 key
---@field F10 sf.Keyboard.Key
--- The F11 key
---@field F11 sf.Keyboard.Key
--- The F12 key
---@field F12 sf.Keyboard.Key
--- The F13 key
---@field F13 sf.Keyboard.Key
--- The F14 key
---@field F14 sf.Keyboard.Key
--- The F15 key
---@field F15 sf.Keyboard.Key
--- The Pause key
---@field Pause sf.Keyboard.Key
sf.Keyboard = sf.Keyboard or {}
sf.Keyboard.Key = sf.Keyboard.Key or {}
--- @brief The total number of keyboard keys, ignoring `Key::Unknown`
---@type integer
sf.Keyboard.KeyCount = nil
--- @brief Scancodes
---
--- The enumerators are bound to a physical key and do not depend on
--- the keyboard layout used by the operating system. Usually, the AT-101
--- keyboard can be used as reference for the physical position of the keys.
---@class sf.Keyboard.Scan
--- Represents any scancode not present in this enum
---@field Unknown sf.Keyboard.Scan
--- Keyboard a and A key
---@field A sf.Keyboard.Scan
--- Keyboard b and B key
---@field B sf.Keyboard.Scan
--- Keyboard c and C key
---@field C sf.Keyboard.Scan
--- Keyboard d and D key
---@field D sf.Keyboard.Scan
--- Keyboard e and E key
---@field E sf.Keyboard.Scan
--- Keyboard f and F key
---@field F sf.Keyboard.Scan
--- Keyboard g and G key
---@field G sf.Keyboard.Scan
--- Keyboard h and H key
---@field H sf.Keyboard.Scan
--- Keyboard i and I key
---@field I sf.Keyboard.Scan
--- Keyboard j and J key
---@field J sf.Keyboard.Scan
--- Keyboard k and K key
---@field K sf.Keyboard.Scan
--- Keyboard l and L key
---@field L sf.Keyboard.Scan
--- Keyboard m and M key
---@field M sf.Keyboard.Scan
--- Keyboard n and N key
---@field N sf.Keyboard.Scan
--- Keyboard o and O key
---@field O sf.Keyboard.Scan
--- Keyboard p and P key
---@field P sf.Keyboard.Scan
--- Keyboard q and Q key
---@field Q sf.Keyboard.Scan
--- Keyboard r and R key
---@field R sf.Keyboard.Scan
--- Keyboard s and S key
---@field S sf.Keyboard.Scan
--- Keyboard t and T key
---@field T sf.Keyboard.Scan
--- Keyboard u and U key
---@field U sf.Keyboard.Scan
--- Keyboard v and V key
---@field V sf.Keyboard.Scan
--- Keyboard w and W key
---@field W sf.Keyboard.Scan
--- Keyboard x and X key
---@field X sf.Keyboard.Scan
--- Keyboard y and Y key
---@field Y sf.Keyboard.Scan
--- Keyboard z and Z key
---@field Z sf.Keyboard.Scan
--- Keyboard 1 and ! key
---@field Num1 sf.Keyboard.Scan
--- Keyboard 2 and @ key
---@field Num2 sf.Keyboard.Scan
--- Keyboard 3 and # key
---@field Num3 sf.Keyboard.Scan
--- Keyboard 4 and $ key
---@field Num4 sf.Keyboard.Scan
--- Keyboard 5 and % key
---@field Num5 sf.Keyboard.Scan
--- Keyboard 6 and ^ key
---@field Num6 sf.Keyboard.Scan
--- Keyboard 7 and & key
---@field Num7 sf.Keyboard.Scan
--- Keyboard 8 and * key
---@field Num8 sf.Keyboard.Scan
--- Keyboard 9 and ) key
---@field Num9 sf.Keyboard.Scan
--- Keyboard 0 and ) key
---@field Num0 sf.Keyboard.Scan
--- Keyboard Enter/Return key
---@field Enter sf.Keyboard.Scan
--- Keyboard Escape key
---@field Escape sf.Keyboard.Scan
--- Keyboard Backspace key
---@field Backspace sf.Keyboard.Scan
--- Keyboard Tab key
---@field Tab sf.Keyboard.Scan
--- Keyboard Space key
---@field Space sf.Keyboard.Scan
--- Keyboard - and _ key
---@field Hyphen sf.Keyboard.Scan
--- Keyboard = and +
---@field Equal sf.Keyboard.Scan
--- Keyboard [ and { key
---@field LBracket sf.Keyboard.Scan
--- Keyboard ] and } key
---@field RBracket sf.Keyboard.Scan
--- Keyboard \ and | key OR various keys for Non-US keyboards
---@field Backslash sf.Keyboard.Scan
--- Keyboard ; and : key
---@field Semicolon sf.Keyboard.Scan
--- Keyboard ' and " key
---@field Apostrophe sf.Keyboard.Scan
--- Keyboard ` and ~ key
---@field Grave sf.Keyboard.Scan
--- Keyboard , and < key
---@field Comma sf.Keyboard.Scan
--- Keyboard . and > key
---@field Period sf.Keyboard.Scan
--- Keyboard / and ? key
---@field Slash sf.Keyboard.Scan
--- Keyboard F1 key
---@field F1 sf.Keyboard.Scan
--- Keyboard F2 key
---@field F2 sf.Keyboard.Scan
--- Keyboard F3 key
---@field F3 sf.Keyboard.Scan
--- Keyboard F4 key
---@field F4 sf.Keyboard.Scan
--- Keyboard F5 key
---@field F5 sf.Keyboard.Scan
--- Keyboard F6 key
---@field F6 sf.Keyboard.Scan
--- Keyboard F7 key
---@field F7 sf.Keyboard.Scan
--- Keyboard F8 key
---@field F8 sf.Keyboard.Scan
--- Keyboard F9 key
---@field F9 sf.Keyboard.Scan
--- Keyboard F10 key
---@field F10 sf.Keyboard.Scan
--- Keyboard F11 key
---@field F11 sf.Keyboard.Scan
--- Keyboard F12 key
---@field F12 sf.Keyboard.Scan
--- Keyboard F13 key
---@field F13 sf.Keyboard.Scan
--- Keyboard F14 key
---@field F14 sf.Keyboard.Scan
--- Keyboard F15 key
---@field F15 sf.Keyboard.Scan
--- Keyboard F16 key
---@field F16 sf.Keyboard.Scan
--- Keyboard F17 key
---@field F17 sf.Keyboard.Scan
--- Keyboard F18 key
---@field F18 sf.Keyboard.Scan
--- Keyboard F19 key
---@field F19 sf.Keyboard.Scan
--- Keyboard F20 key
---@field F20 sf.Keyboard.Scan
--- Keyboard F21 key
---@field F21 sf.Keyboard.Scan
--- Keyboard F22 key
---@field F22 sf.Keyboard.Scan
--- Keyboard F23 key
---@field F23 sf.Keyboard.Scan
--- Keyboard F24 key
---@field F24 sf.Keyboard.Scan
--- Keyboard Caps %Lock key
---@field CapsLock sf.Keyboard.Scan
--- Keyboard Print Screen key
---@field PrintScreen sf.Keyboard.Scan
--- Keyboard Scroll %Lock key
---@field ScrollLock sf.Keyboard.Scan
--- Keyboard Pause key
---@field Pause sf.Keyboard.Scan
--- Keyboard Insert key
---@field Insert sf.Keyboard.Scan
--- Keyboard Home key
---@field Home sf.Keyboard.Scan
--- Keyboard Page Up key
---@field PageUp sf.Keyboard.Scan
--- Keyboard Delete Forward key
---@field Delete sf.Keyboard.Scan
--- Keyboard End key
---@field End sf.Keyboard.Scan
--- Keyboard Page Down key
---@field PageDown sf.Keyboard.Scan
--- Keyboard Right Arrow key
---@field Right sf.Keyboard.Scan
--- Keyboard Left Arrow key
---@field Left sf.Keyboard.Scan
--- Keyboard Down Arrow key
---@field Down sf.Keyboard.Scan
--- Keyboard Up Arrow key
---@field Up sf.Keyboard.Scan
--- Keypad Num %Lock and Clear key
---@field NumLock sf.Keyboard.Scan
--- Keypad / key
---@field NumpadDivide sf.Keyboard.Scan
--- Keypad * key
---@field NumpadMultiply sf.Keyboard.Scan
--- Keypad - key
---@field NumpadMinus sf.Keyboard.Scan
--- Keypad + key
---@field NumpadPlus sf.Keyboard.Scan
--- keypad = key
---@field NumpadEqual sf.Keyboard.Scan
--- Keypad Enter/Return key
---@field NumpadEnter sf.Keyboard.Scan
--- Keypad . and Delete key
---@field NumpadDecimal sf.Keyboard.Scan
--- Keypad 1 and End key
---@field Numpad1 sf.Keyboard.Scan
--- Keypad 2 and Down Arrow key
---@field Numpad2 sf.Keyboard.Scan
--- Keypad 3 and Page Down key
---@field Numpad3 sf.Keyboard.Scan
--- Keypad 4 and Left Arrow key
---@field Numpad4 sf.Keyboard.Scan
--- Keypad 5 key
---@field Numpad5 sf.Keyboard.Scan
--- Keypad 6 and Right Arrow key
---@field Numpad6 sf.Keyboard.Scan
--- Keypad 7 and Home key
---@field Numpad7 sf.Keyboard.Scan
--- Keypad 8 and Up Arrow key
---@field Numpad8 sf.Keyboard.Scan
--- Keypad 9 and Page Up key
---@field Numpad9 sf.Keyboard.Scan
--- Keypad 0 and Insert key
---@field Numpad0 sf.Keyboard.Scan
--- Keyboard Non-US \ and | key
---@field NonUsBackslash sf.Keyboard.Scan
--- Keyboard Application key
---@field Application sf.Keyboard.Scan
--- Keyboard Execute key
---@field Execute sf.Keyboard.Scan
--- Keyboard Mode Change key
---@field ModeChange sf.Keyboard.Scan
--- Keyboard Help key
---@field Help sf.Keyboard.Scan
--- Keyboard Menu key
---@field Menu sf.Keyboard.Scan
--- Keyboard Select key
---@field Select sf.Keyboard.Scan
--- Keyboard Redo key
---@field Redo sf.Keyboard.Scan
--- Keyboard Undo key
---@field Undo sf.Keyboard.Scan
--- Keyboard Cut key
---@field Cut sf.Keyboard.Scan
--- Keyboard Copy key
---@field Copy sf.Keyboard.Scan
--- Keyboard Paste key
---@field Paste sf.Keyboard.Scan
--- Keyboard Volume Mute key
---@field VolumeMute sf.Keyboard.Scan
--- Keyboard Volume Up key
---@field VolumeUp sf.Keyboard.Scan
--- Keyboard Volume Down key
---@field VolumeDown sf.Keyboard.Scan
--- Keyboard Media Play Pause key
---@field MediaPlayPause sf.Keyboard.Scan
--- Keyboard Media Stop key
---@field MediaStop sf.Keyboard.Scan
--- Keyboard Media Next Track key
---@field MediaNextTrack sf.Keyboard.Scan
--- Keyboard Media Previous Track key
---@field MediaPreviousTrack sf.Keyboard.Scan
--- Keyboard Left Control key
---@field LControl sf.Keyboard.Scan
--- Keyboard Left Shift key
---@field LShift sf.Keyboard.Scan
--- Keyboard Left Alt key
---@field LAlt sf.Keyboard.Scan
--- Keyboard Left System key
---@field LSystem sf.Keyboard.Scan
--- Keyboard Right Control key
---@field RControl sf.Keyboard.Scan
--- Keyboard Right Shift key
---@field RShift sf.Keyboard.Scan
--- Keyboard Right Alt key
---@field RAlt sf.Keyboard.Scan
--- Keyboard Right System key
---@field RSystem sf.Keyboard.Scan
--- Keyboard Back key
---@field Back sf.Keyboard.Scan
--- Keyboard Forward key
---@field Forward sf.Keyboard.Scan
--- Keyboard Refresh key
---@field Refresh sf.Keyboard.Scan
--- Keyboard Stop key
---@field Stop sf.Keyboard.Scan
--- Keyboard Search key
---@field Search sf.Keyboard.Scan
--- Keyboard Favorites key
---@field Favorites sf.Keyboard.Scan
--- Keyboard Home Page key
---@field HomePage sf.Keyboard.Scan
--- Keyboard Launch Application 1 key
---@field LaunchApplication1 sf.Keyboard.Scan
--- Keyboard Launch Application 2 key
---@field LaunchApplication2 sf.Keyboard.Scan
--- Keyboard Launch Mail key
---@field LaunchMail sf.Keyboard.Scan
--- Keyboard Launch Media Select key
---@field LaunchMediaSelect sf.Keyboard.Scan
sf.Keyboard.Scan = sf.Keyboard.Scan or {}
---@alias sf.Keyboard.Scancode sf.Keyboard.Scan
--- @brief The total number of scancodes, ignoring `Scan::Unknown`
---@type integer
sf.Keyboard.ScancodeCount = nil
--- @brief Check if a key is pressed
---
--- @warning On macOS you're required to grant input monitoring access for
--- your application in order for `isKeyPressed` to work.
---
--- @param key Key to check
---
--- @return `true` if the key is pressed, `false` otherwise
---@overload fun(code: sf.Keyboard.Scan): boolean
---@param key sf.Keyboard.Key
---@return boolean
function sf.Keyboard.isKeyPressed(key) end
--- @brief Localize a physical key to a logical one
---
--- @param code Scancode to localize
---
--- @return The key corresponding to the scancode under the current
--- keyboard layout used by the operating system, or
--- `sf::Keyboard::Key::Unknown` when the scancode cannot be mapped
--- to a Key.
---
--- @see `delocalize`
---@type fun(code: sf.Keyboard.Scan): sf.Keyboard.Key
sf.Keyboard.localize = function() end
--- @brief Identify the physical key corresponding to a logical one
---
--- @param key Key to "delocalize"
---
--- @return The scancode corresponding to the key under the current
--- keyboard layout used by the operating system, or
--- `sf::Keyboard::Scan::Unknown` when the key cannot be mapped
--- to a `sf::Keyboard::Scancode`.
---
--- @see `localize`
---@type fun(key: sf.Keyboard.Key): sf.Keyboard.Scancode
sf.Keyboard.delocalize = function() end
--- @brief Provide a string representation for a given scancode
---
--- The returned string is a short, non-technical description of
--- the key represented with the given scancode. Most effectively
--- used in user interfaces, as the description for the key takes
--- the users keyboard layout into consideration.
---
--- @warning The result is OS-dependent: for example, `sf::Keyboard::Scan::LSystem`
--- is "Left Meta" on Linux, "Left Windows" on Windows and
--- "Left Command" on macOS.
---
--- The current keyboard layout set by the operating system is used to
--- interpret the scancode: for example, `sf::Keyboard::Key::Semicolon` is
--- mapped to ";" for layout and to "é" for others.
---
--- @param code Scancode to check
---
--- @return The localized description of the code
---@type fun(code: sf.Keyboard.Scan): string
sf.Keyboard.getDescription = function() end
--- @brief Show or hide the virtual keyboard
---
--- @warning The virtual keyboard is not supported on all
--- systems. It will typically be implemented on mobile OSes
--- (Android, iOS) but not on desktop OSes (Windows, Linux, ...).
---
--- If the virtual keyboard is not available, this function does
--- nothing.
---
--- @param visible `true` to show, `false` to hide
---@type fun(visible: boolean)
sf.Keyboard.setVirtualKeyboardVisible = function() end
--- @brief Mouse buttons
---@class sf.Mouse.Button
--- The left mouse button
---@field Left sf.Mouse.Button
--- The right mouse button
---@field Right sf.Mouse.Button
--- The middle (wheel) mouse button
---@field Middle sf.Mouse.Button
--- The first extra mouse button
---@field Extra1 sf.Mouse.Button
--- The second extra mouse button
---@field Extra2 sf.Mouse.Button
sf.Mouse = sf.Mouse or {}
sf.Mouse.Button = sf.Mouse.Button or {}
--- The total number of mouse buttons
---@type integer
sf.Mouse.ButtonCount = nil
--- @brief Mouse wheels
---@class sf.Mouse.Wheel
--- The vertical mouse wheel
---@field Vertical sf.Mouse.Wheel
--- The horizontal mouse wheel
---@field Horizontal sf.Mouse.Wheel
sf.Mouse.Wheel = sf.Mouse.Wheel or {}
--- @brief Check if a mouse button is pressed
---
--- @warning Checking the state of buttons `Mouse::Button::Extra1` and
--- `Mouse::Button::Extra2` is not supported on Linux with X11.
---
--- @param button Button to check
---
--- @return `true` if the button is pressed, `false` otherwise
---@type fun(button: sf.Mouse.Button): boolean
sf.Mouse.isButtonPressed = function() end
--- @brief Get the current position of the mouse in window coordinates
---
--- This function returns the current position of the mouse
--- cursor, relative to the given window.
---
--- @param relativeTo Reference window
---
--- @return Current position of the mouse
---@overload fun(): sf.Vector2i
---@param relativeTo sf.WindowBase
---@return sf.Vector2i
function sf.Mouse.getPosition(relativeTo) end
--- @brief Set the current position of the mouse in window coordinates
---
--- This function sets the current position of the mouse
--- cursor, relative to the given window.
---
--- @param position New position of the mouse
--- @param relativeTo Reference window
---
--- @warning On macOS the OS API used for `setPosition` requires granting
--- of Accessibility permission for your application.
--- See also: https://support.apple.com/guide/mac-help/allow-accessibility-apps-to-access-your-mac-mh43185/
---@overload fun(position: sf.Vector2i)
---@param position sf.Vector2i
---@param relativeTo sf.WindowBase
function sf.Mouse.setPosition(position, relativeTo) end
--- @brief Sensor type
---@class sf.Sensor.Type
--- Measures the raw acceleration (m/s^2)
---@field Accelerometer sf.Sensor.Type
--- Measures the raw rotation rates (radians/s)
---@field Gyroscope sf.Sensor.Type
--- Measures the ambient magnetic field (micro-teslas)
---@field Magnetometer sf.Sensor.Type
--- Measures the direction and intensity of gravity, independent of device acceleration (m/s^2)
---@field Gravity sf.Sensor.Type
--- Measures the direction and intensity of device acceleration, independent of the gravity (m/s^2)
---@field UserAcceleration sf.Sensor.Type
--- Measures the absolute 3D orientation (radians)
---@field Orientation sf.Sensor.Type
sf.Sensor = sf.Sensor or {}
sf.Sensor.Type = sf.Sensor.Type or {}
--- The total number of sensor types
---@type integer
sf.Sensor.Count = nil
--- @brief Check if a sensor is available on the underlying platform
---
--- @param sensor Sensor to check
---
--- @return `true` if the sensor is available, `false` otherwise
---@type fun(sensor: sf.Sensor.Type): boolean
sf.Sensor.isAvailable = function() end
--- @brief Enable or disable a sensor
---
--- All sensors are disabled by default, to avoid consuming too
--- much battery power. Once a sensor is enabled, it starts
--- sending events of the corresponding type.
---
--- This function does nothing if the sensor is unavailable.
---
--- @param sensor  Sensor to enable
--- @param enabled `true` to enable, `false` to disable
---@type fun(sensor: sf.Sensor.Type, enabled: boolean)
sf.Sensor.setEnabled = function() end
--- @brief Get the current sensor value
---
--- @param sensor Sensor to read
---
--- @return The current sensor value
---@type fun(sensor: sf.Sensor.Type): sf.Vector3f
sf.Sensor.getValue = function() end

---@class sf.Event_Closed
sf.Event_Closed = sf.Event_Closed or {}
---@type fun(): sf.Event_Closed
sf.Event_Closed.new = function() end

---@class sf.Event_FocusLost
sf.Event_FocusLost = sf.Event_FocusLost or {}
---@type fun(): sf.Event_FocusLost
sf.Event_FocusLost.new = function() end

---@class sf.Event_FocusGained
sf.Event_FocusGained = sf.Event_FocusGained or {}
---@type fun(): sf.Event_FocusGained
sf.Event_FocusGained.new = function() end

---@class sf.Event_MouseEntered
sf.Event_MouseEntered = sf.Event_MouseEntered or {}
---@type fun(): sf.Event_MouseEntered
sf.Event_MouseEntered.new = function() end

---@class sf.Event_MouseLeft
sf.Event_MouseLeft = sf.Event_MouseLeft or {}
---@type fun(): sf.Event_MouseLeft
sf.Event_MouseLeft.new = function() end

---@class sf.Event_Resized
---@field size sf.Vector2u
sf.Event_Resized = sf.Event_Resized or {}
---@type fun(): sf.Event_Resized
sf.Event_Resized.new = function() end

---@class sf.Event_TextEntered
---@field unicode integer
sf.Event_TextEntered = sf.Event_TextEntered or {}
---@type fun(): sf.Event_TextEntered
sf.Event_TextEntered.new = function() end

---@class sf.Event_KeyPressed
---@field code sf.Keyboard.Key
---@field scancode sf.Keyboard.Scancode
---@field alt boolean
---@field control boolean
---@field shift boolean
---@field system boolean
sf.Event_KeyPressed = sf.Event_KeyPressed or {}
---@type fun(): sf.Event_KeyPressed
sf.Event_KeyPressed.new = function() end

---@class sf.Event_KeyReleased
---@field code sf.Keyboard.Key
---@field scancode sf.Keyboard.Scancode
---@field alt boolean
---@field control boolean
---@field shift boolean
---@field system boolean
sf.Event_KeyReleased = sf.Event_KeyReleased or {}
---@type fun(): sf.Event_KeyReleased
sf.Event_KeyReleased.new = function() end

---@class sf.Event_MouseWheelScrolled
---@field wheel sf.Mouse.Wheel
---@field delta number
---@field position sf.Vector2i
sf.Event_MouseWheelScrolled = sf.Event_MouseWheelScrolled or {}
---@type fun(): sf.Event_MouseWheelScrolled
sf.Event_MouseWheelScrolled.new = function() end

---@class sf.Event_MouseButtonPressed
---@field button sf.Mouse.Button
---@field position sf.Vector2i
sf.Event_MouseButtonPressed = sf.Event_MouseButtonPressed or {}
---@type fun(): sf.Event_MouseButtonPressed
sf.Event_MouseButtonPressed.new = function() end

---@class sf.Event_MouseButtonReleased
---@field button sf.Mouse.Button
---@field position sf.Vector2i
sf.Event_MouseButtonReleased = sf.Event_MouseButtonReleased or {}
---@type fun(): sf.Event_MouseButtonReleased
sf.Event_MouseButtonReleased.new = function() end

---@class sf.Event_MouseMoved
---@field position sf.Vector2i
sf.Event_MouseMoved = sf.Event_MouseMoved or {}
---@type fun(): sf.Event_MouseMoved
sf.Event_MouseMoved.new = function() end

---@class sf.Event_MouseMovedRaw
---@field delta sf.Vector2i
sf.Event_MouseMovedRaw = sf.Event_MouseMovedRaw or {}
---@type fun(): sf.Event_MouseMovedRaw
sf.Event_MouseMovedRaw.new = function() end

---@class sf.Event_JoystickButtonPressed
---@field joystickId integer
---@field button integer
sf.Event_JoystickButtonPressed = sf.Event_JoystickButtonPressed or {}
---@type fun(): sf.Event_JoystickButtonPressed
sf.Event_JoystickButtonPressed.new = function() end

---@class sf.Event_JoystickButtonReleased
---@field joystickId integer
---@field button integer
sf.Event_JoystickButtonReleased = sf.Event_JoystickButtonReleased or {}
---@type fun(): sf.Event_JoystickButtonReleased
sf.Event_JoystickButtonReleased.new = function() end

---@class sf.Event_JoystickMoved
---@field joystickId integer
---@field axis sf.Joystick.Axis
---@field position number
sf.Event_JoystickMoved = sf.Event_JoystickMoved or {}
---@type fun(): sf.Event_JoystickMoved
sf.Event_JoystickMoved.new = function() end

---@class sf.Event_JoystickConnected
---@field joystickId integer
sf.Event_JoystickConnected = sf.Event_JoystickConnected or {}
---@type fun(): sf.Event_JoystickConnected
sf.Event_JoystickConnected.new = function() end

---@class sf.Event_JoystickDisconnected
---@field joystickId integer
sf.Event_JoystickDisconnected = sf.Event_JoystickDisconnected or {}
---@type fun(): sf.Event_JoystickDisconnected
sf.Event_JoystickDisconnected.new = function() end

---@class sf.Event_TouchBegan
---@field finger integer
---@field position sf.Vector2i
sf.Event_TouchBegan = sf.Event_TouchBegan or {}
---@type fun(): sf.Event_TouchBegan
sf.Event_TouchBegan.new = function() end

---@class sf.Event_TouchMoved
---@field finger integer
---@field position sf.Vector2i
sf.Event_TouchMoved = sf.Event_TouchMoved or {}
---@type fun(): sf.Event_TouchMoved
sf.Event_TouchMoved.new = function() end

---@class sf.Event_TouchEnded
---@field finger integer
---@field position sf.Vector2i
sf.Event_TouchEnded = sf.Event_TouchEnded or {}
---@type fun(): sf.Event_TouchEnded
sf.Event_TouchEnded.new = function() end

---@class sf.Event_SensorChanged
---@field type sf.Sensor.Type
---@field value sf.Vector3f
sf.Event_SensorChanged = sf.Event_SensorChanged or {}
---@type fun(): sf.Event_SensorChanged
sf.Event_SensorChanged.new = function() end

---@class sf.Event
sf.Event = sf.Event or {}
---@overload fun(value: sf.Event_Resized): sf.Event
---@overload fun(value: sf.Event_FocusLost): sf.Event
---@overload fun(value: sf.Event_FocusGained): sf.Event
---@overload fun(value: sf.Event_TextEntered): sf.Event
---@overload fun(value: sf.Event_KeyPressed): sf.Event
---@overload fun(value: sf.Event_KeyReleased): sf.Event
---@overload fun(value: sf.Event_MouseWheelScrolled): sf.Event
---@overload fun(value: sf.Event_MouseButtonPressed): sf.Event
---@overload fun(value: sf.Event_MouseButtonReleased): sf.Event
---@overload fun(value: sf.Event_MouseMoved): sf.Event
---@overload fun(value: sf.Event_MouseMovedRaw): sf.Event
---@overload fun(value: sf.Event_MouseEntered): sf.Event
---@overload fun(value: sf.Event_MouseLeft): sf.Event
---@overload fun(value: sf.Event_JoystickButtonPressed): sf.Event
---@overload fun(value: sf.Event_JoystickButtonReleased): sf.Event
---@overload fun(value: sf.Event_JoystickMoved): sf.Event
---@overload fun(value: sf.Event_JoystickConnected): sf.Event
---@overload fun(value: sf.Event_JoystickDisconnected): sf.Event
---@overload fun(value: sf.Event_TouchBegan): sf.Event
---@overload fun(value: sf.Event_TouchMoved): sf.Event
---@overload fun(value: sf.Event_TouchEnded): sf.Event
---@overload fun(value: sf.Event_SensorChanged): sf.Event
---@param value sf.Event_Closed
---@return sf.Event
function sf.Event.new(value) end
---@type fun(self: sf.Event): string
sf.Event.type = function() end
---@type fun(self: sf.Event): any
sf.Event.get = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isClosed = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isResized = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isFocusLost = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isFocusGained = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isTextEntered = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isKeyPressed = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isKeyReleased = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseWheelScrolled = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseButtonPressed = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseButtonReleased = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseMoved = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseMovedRaw = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseEntered = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isMouseLeft = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isJoystickButtonPressed = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isJoystickButtonReleased = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isJoystickMoved = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isJoystickConnected = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isJoystickDisconnected = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isTouchBegan = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isTouchMoved = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isTouchEnded = function() end
---@type fun(self: sf.Event): boolean
sf.Event.isSensorChanged = function() end
---@type fun(self: sf.Event): sf.Event_Closed|nil
sf.Event.getIfClosed = function() end
---@type fun(self: sf.Event): sf.Event_Resized|nil
sf.Event.getIfResized = function() end
---@type fun(self: sf.Event): sf.Event_FocusLost|nil
sf.Event.getIfFocusLost = function() end
---@type fun(self: sf.Event): sf.Event_FocusGained|nil
sf.Event.getIfFocusGained = function() end
---@type fun(self: sf.Event): sf.Event_TextEntered|nil
sf.Event.getIfTextEntered = function() end
---@type fun(self: sf.Event): sf.Event_KeyPressed|nil
sf.Event.getIfKeyPressed = function() end
---@type fun(self: sf.Event): sf.Event_KeyReleased|nil
sf.Event.getIfKeyReleased = function() end
---@type fun(self: sf.Event): sf.Event_MouseWheelScrolled|nil
sf.Event.getIfMouseWheelScrolled = function() end
---@type fun(self: sf.Event): sf.Event_MouseButtonPressed|nil
sf.Event.getIfMouseButtonPressed = function() end
---@type fun(self: sf.Event): sf.Event_MouseButtonReleased|nil
sf.Event.getIfMouseButtonReleased = function() end
---@type fun(self: sf.Event): sf.Event_MouseMoved|nil
sf.Event.getIfMouseMoved = function() end
---@type fun(self: sf.Event): sf.Event_MouseMovedRaw|nil
sf.Event.getIfMouseMovedRaw = function() end
---@type fun(self: sf.Event): sf.Event_MouseEntered|nil
sf.Event.getIfMouseEntered = function() end
---@type fun(self: sf.Event): sf.Event_MouseLeft|nil
sf.Event.getIfMouseLeft = function() end
---@type fun(self: sf.Event): sf.Event_JoystickButtonPressed|nil
sf.Event.getIfJoystickButtonPressed = function() end
---@type fun(self: sf.Event): sf.Event_JoystickButtonReleased|nil
sf.Event.getIfJoystickButtonReleased = function() end
---@type fun(self: sf.Event): sf.Event_JoystickMoved|nil
sf.Event.getIfJoystickMoved = function() end
---@type fun(self: sf.Event): sf.Event_JoystickConnected|nil
sf.Event.getIfJoystickConnected = function() end
---@type fun(self: sf.Event): sf.Event_JoystickDisconnected|nil
sf.Event.getIfJoystickDisconnected = function() end
---@type fun(self: sf.Event): sf.Event_TouchBegan|nil
sf.Event.getIfTouchBegan = function() end
---@type fun(self: sf.Event): sf.Event_TouchMoved|nil
sf.Event.getIfTouchMoved = function() end
---@type fun(self: sf.Event): sf.Event_TouchEnded|nil
sf.Event.getIfTouchEnded = function() end
---@type fun(self: sf.Event): sf.Event_SensorChanged|nil
sf.Event.getIfSensorChanged = function() end
sf.Touch = sf.Touch or {}
--- @brief Check if a touch event is currently down
---
--- @deprecated Use `sf::Event::TouchBegan` and `sf::Event::TouchEnded`
---
--- @param finger Finger index
---
--- @return `true` if @a finger is currently touching the screen, `false` otherwise
---@type fun(finger: integer): boolean
sf.Touch.isDown = function() end
--- @brief Get the current position of a touch in window coordinates
---
--- This function returns the current touch position
--- relative to the given window.
---
--- @deprecated Use position member of `sf::Event::TouchBegan`, `sf::Event::TouchEnded` and `sf::Event::TouchMoved`
---
--- @param finger Finger index
--- @param relativeTo Reference window
---
--- @return Current position of @a finger, or undefined if it's not down
---@overload fun(finger: integer): sf.Vector2i
---@param finger integer
---@param relativeTo sf.WindowBase
---@return sf.Vector2i
function sf.Touch.getPosition(finger, relativeTo) end
--- @brief VideoMode defines a video mode (size, bpp)
---@class sf.VideoMode
--- Video mode width and height, in pixels
---@field size sf.Vector2u
--- Video mode pixel depth, in bits per pixels
---@field bitsPerPixel integer
sf.VideoMode = sf.VideoMode or {}
--- @brief Construct the video mode with its attributes
---
--- @param modeSize         Width and height in pixels
--- @param modeBitsPerPixel Pixel depths in bits per pixel
---@overload fun(modeSize: sf.Vector2u): sf.VideoMode
---@overload fun(): sf.VideoMode
---@param modeSize sf.Vector2u
---@param modeBitsPerPixel integer
---@return sf.VideoMode
function sf.VideoMode.new(modeSize, modeBitsPerPixel) end
--- @brief Get the current desktop video mode
---
--- @return Current desktop video mode
---@type fun(): sf.VideoMode
sf.VideoMode.getDesktopMode = function() end
--- @brief Retrieve all the video modes supported in fullscreen mode
---
--- When creating a fullscreen window, the video mode is restricted
--- to be compatible with what the graphics driver and monitor
--- support. This function returns the complete list of all video
--- modes that can be used in fullscreen mode.
--- The returned array is sorted from best to worst, so that
--- the first element will always give the best mode (higher
--- width, height and bits-per-pixel).
---
--- @return Array containing all the supported fullscreen modes
---@type fun(): sf.VideoMode[]
sf.VideoMode.getFullscreenModes = function() end
--- @brief Tell whether or not the video mode is valid
---
--- The validity of video modes is only relevant when using
--- fullscreen windows; otherwise any video mode can be used
--- with no restriction.
---
--- @return `true` if the video mode is valid for fullscreen mode
---@type fun(self: sf.VideoMode): boolean
sf.VideoMode.isValid = function() end
sf.Style = sf.Style or {}
--- No border / title bar (this flag and all others are mutually exclusive)
---@type integer
sf.Style.None = nil
--- Title bar + fixed border
---@type integer
sf.Style.Titlebar = nil
--- Title bar + resizable border + maximize button
---@type integer
sf.Style.Resize = nil
--- Title bar + close button (see note)
---@type integer
sf.Style.Close = nil
--- Default window style
---@type integer
sf.Style.Default = nil
--- @ingroup window
--- @brief Enumeration of the window states
---@class sf.State
--- Floating window
---@field Windowed sf.State
--- Fullscreen window
---@field Fullscreen sf.State
sf.State = sf.State or {}
--- @brief Window that serves as a base for other windows
---@class sf.WindowBase
sf.WindowBase = sf.WindowBase or {}
--- @brief Construct a new window
---
--- This constructor creates the window with the size and pixel
--- depth defined in `mode`. An optional style can be passed to
--- customize the look and behavior of the window (borders,
--- title bar, resizable, closable, ...). An optional state can
--- be provided. If `state` is `State::Fullscreen`, then `mode`
--- must be a valid video mode.
---
--- @param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title Title of the window
--- @param style %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state %Window state
---@overload fun(mode: sf.VideoMode, title: string, style: integer): sf.WindowBase
---@overload fun(mode: sf.VideoMode, title: string, state: sf.State): sf.WindowBase
---@overload fun(mode: sf.VideoMode, title: string): sf.WindowBase
---@overload fun(handle: nil): sf.WindowBase
---@overload fun(): sf.WindowBase
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
---@return sf.WindowBase
function sf.WindowBase.new(mode, title, style, state) end
--- @brief Create (or recreate) the window
---
--- If the window was already created, it closes it first.
--- If `state` is `State::Fullscreen`, then `mode` must be
--- a valid video mode.
---
--- @param mode  Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title Title of the window
--- @param style %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state %Window state
---@overload fun(self: sf.WindowBase, mode: sf.VideoMode, title: string, style: integer)
---@overload fun(self: sf.WindowBase, mode: sf.VideoMode, title: string, state: sf.State)
---@overload fun(self: sf.WindowBase, mode: sf.VideoMode, title: string)
---@overload fun(self: sf.WindowBase, handle: nil)
---@param self sf.WindowBase
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
function sf.WindowBase.create(self, mode, title, style, state) end
--- @brief Close the window and destroy all the attached resources
---
--- After calling this function, the `sf::Window` instance remains
--- valid and you can call `create()` to recreate the window.
--- All other functions such as `pollEvent()` or `display()` will
--- still work (i.e. you don't have to test `isOpen()` every time),
--- and will have no effect on closed windows.
---@type fun(self: sf.WindowBase)
sf.WindowBase.close = function() end
--- @brief Tell whether or not the window is open
---
--- This function returns whether or not the window exists.
--- Note that a hidden window (`setVisible(false)`) is open
--- (therefore this function would return `true`).
---
--- @return `true` if the window is open, `false` if it has been closed
---@type fun(self: sf.WindowBase): boolean
sf.WindowBase.isOpen = function() end
--- @brief Pop the next event from the front of the FIFO event queue, if any, and return it
---
--- This function is not blocking: if there's no pending event then
--- it will return a `std::nullopt`. Note that more than one event
--- may be present in the event queue, thus you should always call
--- this function in a loop to make sure that you process every
--- pending event.
--- @code
--- while (const std::optional event = window.pollEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @return The event, otherwise `std::nullopt` if no events are pending
---
--- @see `waitEvent`, `handleEvents`
---@type fun(self: sf.WindowBase): sf.Event|nil
sf.WindowBase.pollEvent = function() end
--- @brief Wait for an event and return it
---
--- This function is blocking: if there's no pending event then
--- it will wait until an event is received or until the provided
--- timeout elapses. Only if an error or a timeout occurs the
--- returned event will be `std::nullopt`.
--- This function is typically used when you have a thread that is
--- dedicated to events handling: you want to make this thread sleep
--- as long as no new event is received.
--- @code
--- while (const std::optional event = window.waitEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @param timeout Maximum time to wait (`Time::Zero` for infinite)
---
--- @return The event, otherwise `std::nullopt` on timeout or if window was closed
---
--- @see `pollEvent`, `handleEvents`
---@overload fun(self: sf.WindowBase): sf.Event|nil
---@param self sf.WindowBase
---@param timeout sf.Time
---@return sf.Event|nil
function sf.WindowBase.waitEvent(self, timeout) end
--- @brief Get the position of the window
---
--- @return Position of the window, in pixels
---
--- @see `setPosition`
---@type fun(self: sf.WindowBase): sf.Vector2i
sf.WindowBase.getPosition = function() end
--- @brief Change the position of the window on screen
---
--- This function only works for top-level windows
--- (i.e. it will be ignored for windows created from
--- the handle of a child window/control).
---
--- @param position New position, in pixels
---
--- @see `getPosition`
---@type fun(self: sf.WindowBase, position: sf.Vector2i)
sf.WindowBase.setPosition = function() end
--- @brief Get the size of the rendering region of the window
---
--- The size doesn't include the titlebar and borders
--- of the window.
---
--- @return Size in pixels
---
--- @see `setSize`
---@type fun(self: sf.WindowBase): sf.Vector2u
sf.WindowBase.getSize = function() end
--- @brief Change the size of the rendering region of the window
---
--- @param size New size, in pixels
---
--- @see `getSize`
---@type fun(self: sf.WindowBase, size: sf.Vector2u)
sf.WindowBase.setSize = function() end
--- @brief Set the minimum window rendering region size
---
--- Pass `std::nullopt` to unset the minimum size
---
--- @param minimumSize New minimum size, in pixels
---@type fun(self: sf.WindowBase, minimumSize: sf.Vector2u|nil)
sf.WindowBase.setMinimumSize = function() end
--- @brief Set the maximum window rendering region size
---
--- Pass `std::nullopt` to unset the maximum size
---
--- @param maximumSize New maximum size, in pixels
---@type fun(self: sf.WindowBase, maximumSize: sf.Vector2u|nil)
sf.WindowBase.setMaximumSize = function() end
--- @brief Change the title of the window
---
--- @param title New title
---
--- @see `setIcon`
---@type fun(self: sf.WindowBase, title: string)
sf.WindowBase.setTitle = function() end
--- @brief Change the window's icon
---
--- `pixels` must be an array of `size` pixels
--- in 32-bits RGBA format.
---
--- The OS default icon is used by default.
---
--- @param size   Icon's width and height, in pixels
--- @param pixels Pointer to the array of pixels in memory. The
--- pixels are copied, so you need not keep the
--- source alive after calling this function.
---
--- @see `setTitle`
---@type fun(self: sf.WindowBase, size: sf.Vector2u, pixels: any)
sf.WindowBase.setIcon = function() end
--- @brief Show or hide the window
---
--- The window is shown by default.
---
--- @param visible `true` to show the window, `false` to hide it
---@type fun(self: sf.WindowBase, visible: boolean)
sf.WindowBase.setVisible = function() end
--- @brief Show or hide the mouse cursor
---
--- The mouse cursor is visible by default.
---
--- @warning On Windows, this function needs to be called from the
--- thread that created the window.
---
--- @param visible `true` to show the mouse cursor, `false` to hide it
---@type fun(self: sf.WindowBase, visible: boolean)
sf.WindowBase.setMouseCursorVisible = function() end
--- @brief Grab or release the mouse cursor
---
--- If set, grabs the mouse cursor inside this window's client
--- area so it may no longer be moved outside its bounds.
--- Note that grabbing is only active while the window has
--- focus.
---
--- @param grabbed `true` to enable, `false` to disable
---@type fun(self: sf.WindowBase, grabbed: boolean)
sf.WindowBase.setMouseCursorGrabbed = function() end
--- @brief Set the displayed cursor to a native system cursor
---
--- Upon window creation, the arrow cursor is used by default.
---
--- @warning The cursor must not be destroyed while in use by
--- the window.
---
--- @warning Features related to Cursor are not supported on
--- iOS and Android.
---
--- @param cursor Native system cursor type to display
---
--- @see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`
---@type fun(self: sf.WindowBase, cursor: sf.Cursor)
sf.WindowBase.setMouseCursor = function() end
--- @brief Enable or disable automatic key-repeat
---
--- If key repeat is enabled, you will receive repeated
--- KeyPressed events while keeping a key pressed. If it is disabled,
--- you will only get a single event when the key is pressed.
---
--- Key repeat is enabled by default.
---
--- @param enabled `true` to enable, `false` to disable
---@type fun(self: sf.WindowBase, enabled: boolean)
sf.WindowBase.setKeyRepeatEnabled = function() end
--- @brief Change the joystick threshold
---
--- The joystick threshold is the value below which
--- no JoystickMoved event will be generated.
---
--- The threshold value is 0.1 by default.
---
--- @param threshold New threshold, in the range [0, 100]
---@type fun(self: sf.WindowBase, threshold: number)
sf.WindowBase.setJoystickThreshold = function() end
--- @brief Request the current window to be made the active
--- foreground window
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or mouse events.
--- If a window requests focus, it only hints to the operating
--- system, that it would like to be focused. The operating system
--- is free to deny the request.
--- This is not to be confused with `setActive()`.
---
--- @see `hasFocus`
---@type fun(self: sf.WindowBase)
sf.WindowBase.requestFocus = function() end
--- @brief Check whether the window has the input focus
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or most mouse
--- events.
---
--- @return `true` if window has focus, `false` otherwise
--- @see `requestFocus`
---@type fun(self: sf.WindowBase): boolean
sf.WindowBase.hasFocus = function() end
--- @brief Get the OS-specific handle of the window
---
--- The type of the returned handle is `sf::WindowHandle`,
--- which is a type alias to the handle type defined by the OS.
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return System handle of the window
---@type fun(self: sf.WindowBase): sf.WindowHandle
sf.WindowBase.getNativeHandle = function() end
--- @brief Window that serves as a target for OpenGL rendering
---@class sf.Window : sf.WindowBase
sf.Window = sf.Window or {}
--- @brief Construct a new window
---
--- This constructor creates the window with the size and pixel
--- depth defined in `mode`. An optional style can be passed to
--- customize the look and behavior of the window (borders,
--- title bar, resizable, closable, ...). An optional state can
--- be provided. If `state` is `State::Fullscreen`, then `mode`
--- must be a valid video mode.
---
--- The last parameter is an optional structure specifying
--- advanced OpenGL context settings such as anti-aliasing,
--- depth-buffer bits, etc.
---
--- @param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title    Title of the window
--- @param style    %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state    %Window state
--- @param settings Additional settings for the underlying OpenGL context
---@overload fun(mode: sf.VideoMode, title: string, style: integer, state: sf.State): sf.Window
---@overload fun(mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings): sf.Window
---@overload fun(mode: sf.VideoMode, title: string, style: integer): sf.Window
---@overload fun(mode: sf.VideoMode, title: string, state: sf.State): sf.Window
---@overload fun(mode: sf.VideoMode, title: string): sf.Window
---@overload fun(handle: nil, settings: sf.ContextSettings): sf.Window
---@overload fun(handle: nil): sf.Window
---@overload fun(): sf.Window
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
---@param settings sf.ContextSettings
---@return sf.Window
function sf.Window.new(mode, title, style, state, settings) end
--- @brief Create (or recreate) the window
---
--- If the window was already created, it closes it first.
--- If `state` is `State::Fullscreen`, then `mode` must be
--- a valid video mode.
---
--- The last parameter is a structure specifying advanced OpenGL
--- context settings such as anti-aliasing, depth-buffer bits, etc.
---
--- @param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title    Title of the window
--- @param style    %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state    %Window state
--- @param settings Additional settings for the underlying OpenGL context
---@overload fun(self: sf.Window, mode: sf.VideoMode, title: string, style: integer, state: sf.State)
---@overload fun(self: sf.Window, mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings)
---@overload fun(self: sf.Window, mode: sf.VideoMode, title: string, style: integer)
---@overload fun(self: sf.Window, mode: sf.VideoMode, title: string, state: sf.State)
---@overload fun(self: sf.Window, mode: sf.VideoMode, title: string)
---@overload fun(self: sf.Window, handle: nil, settings: sf.ContextSettings)
---@overload fun(self: sf.Window, handle: nil)
---@param self sf.Window
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
---@param settings sf.ContextSettings
function sf.Window.create(self, mode, title, style, state, settings) end
--- @brief Close the window and destroy all the attached resources
---
--- After calling this function, the `sf::Window` instance remains
--- valid and you can call `create()` to recreate the window.
--- All other functions such as `pollEvent()` or `display()` will
--- still work (i.e. you don't have to test `isOpen()` every time),
--- and will have no effect on closed windows.
---@type fun(self: sf.Window)
sf.Window.close = function() end
--- @brief Tell whether or not the window is open
---
--- This function returns whether or not the window exists.
--- Note that a hidden window (`setVisible(false)`) is open
--- (therefore this function would return `true`).
---
--- @return `true` if the window is open, `false` if it has been closed
---@type fun(self: sf.Window): boolean
sf.Window.isOpen = function() end
--- @brief Pop the next event from the front of the FIFO event queue, if any, and return it
---
--- This function is not blocking: if there's no pending event then
--- it will return a `std::nullopt`. Note that more than one event
--- may be present in the event queue, thus you should always call
--- this function in a loop to make sure that you process every
--- pending event.
--- @code
--- while (const std::optional event = window.pollEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @return The event, otherwise `std::nullopt` if no events are pending
---
--- @see `waitEvent`, `handleEvents`
---@type fun(self: sf.Window): sf.Event|nil
sf.Window.pollEvent = function() end
--- @brief Wait for an event and return it
---
--- This function is blocking: if there's no pending event then
--- it will wait until an event is received or until the provided
--- timeout elapses. Only if an error or a timeout occurs the
--- returned event will be `std::nullopt`.
--- This function is typically used when you have a thread that is
--- dedicated to events handling: you want to make this thread sleep
--- as long as no new event is received.
--- @code
--- while (const std::optional event = window.waitEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @param timeout Maximum time to wait (`Time::Zero` for infinite)
---
--- @return The event, otherwise `std::nullopt` on timeout or if window was closed
---
--- @see `pollEvent`, `handleEvents`
---@overload fun(self: sf.Window): sf.Event|nil
---@param self sf.Window
---@param timeout sf.Time
---@return sf.Event|nil
function sf.Window.waitEvent(self, timeout) end
--- @brief Get the position of the window
---
--- @return Position of the window, in pixels
---
--- @see `setPosition`
---@type fun(self: sf.Window): sf.Vector2i
sf.Window.getPosition = function() end
--- @brief Change the position of the window on screen
---
--- This function only works for top-level windows
--- (i.e. it will be ignored for windows created from
--- the handle of a child window/control).
---
--- @param position New position, in pixels
---
--- @see `getPosition`
---@type fun(self: sf.Window, position: sf.Vector2i)
sf.Window.setPosition = function() end
--- @brief Get the size of the rendering region of the window
---
--- The size doesn't include the titlebar and borders
--- of the window.
---
--- @return Size in pixels
---
--- @see `setSize`
---@type fun(self: sf.Window): sf.Vector2u
sf.Window.getSize = function() end
--- @brief Change the size of the rendering region of the window
---
--- @param size New size, in pixels
---
--- @see `getSize`
---@type fun(self: sf.Window, size: sf.Vector2u)
sf.Window.setSize = function() end
--- @brief Set the minimum window rendering region size
---
--- Pass `std::nullopt` to unset the minimum size
---
--- @param minimumSize New minimum size, in pixels
---@type fun(self: sf.Window, minimumSize: sf.Vector2u|nil)
sf.Window.setMinimumSize = function() end
--- @brief Set the maximum window rendering region size
---
--- Pass `std::nullopt` to unset the maximum size
---
--- @param maximumSize New maximum size, in pixels
---@type fun(self: sf.Window, maximumSize: sf.Vector2u|nil)
sf.Window.setMaximumSize = function() end
--- @brief Change the title of the window
---
--- @param title New title
---
--- @see `setIcon`
---@type fun(self: sf.Window, title: string)
sf.Window.setTitle = function() end
--- @brief Change the window's icon
---
--- `pixels` must be an array of `size` pixels
--- in 32-bits RGBA format.
---
--- The OS default icon is used by default.
---
--- @param size   Icon's width and height, in pixels
--- @param pixels Pointer to the array of pixels in memory. The
--- pixels are copied, so you need not keep the
--- source alive after calling this function.
---
--- @see `setTitle`
---@type fun(self: sf.Window, size: sf.Vector2u, pixels: any)
sf.Window.setIcon = function() end
--- @brief Show or hide the window
---
--- The window is shown by default.
---
--- @param visible `true` to show the window, `false` to hide it
---@type fun(self: sf.Window, visible: boolean)
sf.Window.setVisible = function() end
--- @brief Show or hide the mouse cursor
---
--- The mouse cursor is visible by default.
---
--- @warning On Windows, this function needs to be called from the
--- thread that created the window.
---
--- @param visible `true` to show the mouse cursor, `false` to hide it
---@type fun(self: sf.Window, visible: boolean)
sf.Window.setMouseCursorVisible = function() end
--- @brief Grab or release the mouse cursor
---
--- If set, grabs the mouse cursor inside this window's client
--- area so it may no longer be moved outside its bounds.
--- Note that grabbing is only active while the window has
--- focus.
---
--- @param grabbed `true` to enable, `false` to disable
---@type fun(self: sf.Window, grabbed: boolean)
sf.Window.setMouseCursorGrabbed = function() end
--- @brief Set the displayed cursor to a native system cursor
---
--- Upon window creation, the arrow cursor is used by default.
---
--- @warning The cursor must not be destroyed while in use by
--- the window.
---
--- @warning Features related to Cursor are not supported on
--- iOS and Android.
---
--- @param cursor Native system cursor type to display
---
--- @see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`
---@type fun(self: sf.Window, cursor: sf.Cursor)
sf.Window.setMouseCursor = function() end
--- @brief Enable or disable automatic key-repeat
---
--- If key repeat is enabled, you will receive repeated
--- KeyPressed events while keeping a key pressed. If it is disabled,
--- you will only get a single event when the key is pressed.
---
--- Key repeat is enabled by default.
---
--- @param enabled `true` to enable, `false` to disable
---@type fun(self: sf.Window, enabled: boolean)
sf.Window.setKeyRepeatEnabled = function() end
--- @brief Change the joystick threshold
---
--- The joystick threshold is the value below which
--- no JoystickMoved event will be generated.
---
--- The threshold value is 0.1 by default.
---
--- @param threshold New threshold, in the range [0, 100]
---@type fun(self: sf.Window, threshold: number)
sf.Window.setJoystickThreshold = function() end
--- @brief Request the current window to be made the active
--- foreground window
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or mouse events.
--- If a window requests focus, it only hints to the operating
--- system, that it would like to be focused. The operating system
--- is free to deny the request.
--- This is not to be confused with `setActive()`.
---
--- @see `hasFocus`
---@type fun(self: sf.Window)
sf.Window.requestFocus = function() end
--- @brief Check whether the window has the input focus
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or most mouse
--- events.
---
--- @return `true` if window has focus, `false` otherwise
--- @see `requestFocus`
---@type fun(self: sf.Window): boolean
sf.Window.hasFocus = function() end
--- @brief Get the OS-specific handle of the window
---
--- The type of the returned handle is `sf::WindowHandle`,
--- which is a type alias to the handle type defined by the OS.
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return System handle of the window
---@type fun(self: sf.Window): sf.WindowHandle
sf.Window.getNativeHandle = function() end
--- @brief Get the settings of the OpenGL context of the window
---
--- Note that these settings may be different from what was
--- passed to the constructor or the `create()` function,
--- if one or more settings were not supported. In this case,
--- SFML chose the closest match.
---
--- @return Structure containing the OpenGL context settings
---@type fun(self: sf.Window): sf.ContextSettings
sf.Window.getSettings = function() end
--- @brief Enable or disable vertical synchronization
---
--- Activating vertical synchronization will limit the number
--- of frames displayed to the refresh rate of the monitor.
--- This can avoid some visual artifacts, and limit the framerate
--- to a good value (but not constant across different computers).
---
--- Vertical synchronization is disabled by default.
---
--- @param enabled `true` to enable v-sync, `false` to deactivate it
---@type fun(self: sf.Window, enabled: boolean)
sf.Window.setVerticalSyncEnabled = function() end
--- @brief Limit the framerate to a maximum fixed frequency
---
--- If a limit is set, the window will use a small delay after
--- each call to `display()` to ensure that the current frame
--- lasted long enough to match the framerate limit.
--- SFML will try to match the given limit as much as it can,
--- but since it internally uses `sf::sleep`, whose precision
--- depends on the underlying OS, the results may be a little
--- imprecise as well (for example, you can get 65 FPS when
--- requesting 60).
---
--- @param limit Framerate limit, in frames per seconds (use 0 to disable limit)
---@type fun(self: sf.Window, limit: integer)
sf.Window.setFramerateLimit = function() end
--- @brief Activate or deactivate the window as the current target
--- for OpenGL rendering
---
--- A window is active only on the current thread, if you want to
--- make it active on another thread you have to deactivate it
--- on the previous thread first if it was active.
--- Only one window can be active on a thread at a time, thus
--- the window previously active (if any) automatically gets deactivated.
--- This is not to be confused with `requestFocus()`.
---
--- @param active `true` to activate, `false` to deactivate
---
--- @return `true` if operation was successful, `false` otherwise
---@overload fun(self: sf.Window): boolean
---@param self sf.Window
---@param active boolean
---@return boolean
function sf.Window.setActive(self, active) end
--- @brief Display on screen what has been rendered to the window so far
---
--- This function is typically called after all OpenGL rendering
--- has been done for the current frame, in order to show
--- it on screen.
---@type fun(self: sf.Window)
sf.Window.display = function() end
---@alias sf.WindowHandle sf.void*
--- @brief Blending modes for drawing
---@class sf.BlendMode
--- Source blending factor for the color channels
---@field colorSrcFactor sf.BlendMode.Factor
--- Destination blending factor for the color channels
---@field colorDstFactor sf.BlendMode.Factor
--- Blending equation for the color channels
---@field colorEquation sf.BlendMode.Equation
--- Source blending factor for the alpha channel
---@field alphaSrcFactor sf.BlendMode.Factor
--- Destination blending factor for the alpha channel
---@field alphaDstFactor sf.BlendMode.Factor
--- Blending equation for the alpha channel
---@field alphaEquation sf.BlendMode.Equation
sf.BlendMode = sf.BlendMode or {}
--- @brief Construct the blend mode given the factors and equation.
---
--- @param colorSourceFactor      Specifies how to compute the source factor for the color channels.
--- @param colorDestinationFactor Specifies how to compute the destination factor for the color channels.
--- @param colorBlendEquation     Specifies how to combine the source and destination colors.
--- @param alphaSourceFactor      Specifies how to compute the source factor.
--- @param alphaDestinationFactor Specifies how to compute the destination factor.
--- @param alphaBlendEquation     Specifies how to combine the source and destination alphas.
---@overload fun(sourceFactor: sf.BlendMode.Factor, destinationFactor: sf.BlendMode.Factor, blendEquation: sf.BlendMode.Equation): sf.BlendMode
---@overload fun(sourceFactor: sf.BlendMode.Factor, destinationFactor: sf.BlendMode.Factor): sf.BlendMode
---@overload fun(): sf.BlendMode
---@param colorSourceFactor sf.BlendMode.Factor
---@param colorDestinationFactor sf.BlendMode.Factor
---@param colorBlendEquation sf.BlendMode.Equation
---@param alphaSourceFactor sf.BlendMode.Factor
---@param alphaDestinationFactor sf.BlendMode.Factor
---@param alphaBlendEquation sf.BlendMode.Equation
---@return sf.BlendMode
function sf.BlendMode.new(colorSourceFactor, colorDestinationFactor, colorBlendEquation, alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation) end
--- @brief Enumeration of the blending factors
---
--- The factors are mapped directly to their OpenGL equivalents,
--- specified by glBlendFunc() or glBlendFuncSeparate().
---@class sf.BlendMode.Factor
--- (0, 0, 0, 0)
---@field Zero sf.BlendMode.Factor
--- (1, 1, 1, 1)
---@field One sf.BlendMode.Factor
--- (src.r, src.g, src.b, src.a)
---@field SrcColor sf.BlendMode.Factor
--- (1, 1, 1, 1) - (src.r, src.g, src.b, src.a)
---@field OneMinusSrcColor sf.BlendMode.Factor
--- (dst.r, dst.g, dst.b, dst.a)
---@field DstColor sf.BlendMode.Factor
--- (1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)
---@field OneMinusDstColor sf.BlendMode.Factor
--- (src.a, src.a, src.a, src.a)
---@field SrcAlpha sf.BlendMode.Factor
--- (1, 1, 1, 1) - (src.a, src.a, src.a, src.a)
---@field OneMinusSrcAlpha sf.BlendMode.Factor
--- (dst.a, dst.a, dst.a, dst.a)
---@field DstAlpha sf.BlendMode.Factor
--- (1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)
---@field OneMinusDstAlpha sf.BlendMode.Factor
sf.BlendMode.Factor = sf.BlendMode.Factor or {}
--- @brief Enumeration of the blending equations
---
--- The equations are mapped directly to their OpenGL equivalents,
--- specified by glBlendEquation() or glBlendEquationSeparate().
---@class sf.BlendMode.Equation
--- Pixel = Src * SrcFactor + Dst * DstFactor
---@field Add sf.BlendMode.Equation
--- Pixel = Src * SrcFactor - Dst * DstFactor
---@field Subtract sf.BlendMode.Equation
--- Pixel = Dst * DstFactor - Src * SrcFactor
---@field ReverseSubtract sf.BlendMode.Equation
--- Pixel = min(Dst, Src)
---@field Min sf.BlendMode.Equation
--- Pixel = max(Dst, Src)
---@field Max sf.BlendMode.Equation
sf.BlendMode.Equation = sf.BlendMode.Equation or {}
--- Blend source and dest according to dest alpha
---@type sf.BlendMode
sf.BlendAlpha = nil
--- Add source to dest
---@type sf.BlendMode
sf.BlendAdd = nil
--- Multiply source and dest
---@type sf.BlendMode
sf.BlendMultiply = nil
--- Take minimum between source and dest
---@type sf.BlendMode
sf.BlendMin = nil
--- Take maximum between source and dest
---@type sf.BlendMode
sf.BlendMax = nil
--- Overwrite dest with source
---@type sf.BlendMode
sf.BlendNone = nil
--- @brief Utility class for manipulating RGBA colors
---@class sf.Color
--- Red component
---@field r integer
--- Green component
---@field g integer
--- Blue component
---@field b integer
--- Alpha (opacity) component
---@field a integer
sf.Color = sf.Color or {}
--- @brief Construct the color from its 4 RGBA components
---
--- @param red   Red component (in the range [0, 255])
--- @param green Green component (in the range [0, 255])
--- @param blue  Blue component (in the range [0, 255])
--- @param alpha Alpha (opacity) component (in the range [0, 255])
---@overload fun(red: integer, green: integer, blue: integer): sf.Color
---@overload fun(color: integer): sf.Color
---@overload fun(): sf.Color
---@param red integer
---@param green integer
---@param blue integer
---@param alpha integer
---@return sf.Color
function sf.Color.new(red, green, blue, alpha) end
--- @brief Retrieve the color as a 32-bit unsigned integer
---
--- @return Color represented as a 32-bit unsigned integer
---@type fun(self: sf.Color): integer
sf.Color.toInteger = function() end
--- Black predefined color
---@type sf.Color
sf.Color.Black = nil
--- White predefined color
---@type sf.Color
sf.Color.White = nil
--- Red predefined color
---@type sf.Color
sf.Color.Red = nil
--- Green predefined color
---@type sf.Color
sf.Color.Green = nil
--- Blue predefined color
---@type sf.Color
sf.Color.Blue = nil
--- Yellow predefined color
---@type sf.Color
sf.Color.Yellow = nil
--- Magenta predefined color
---@type sf.Color
sf.Color.Magenta = nil
--- Cyan predefined color
---@type sf.Color
sf.Color.Cyan = nil
--- Transparent (black) predefined color
---@type sf.Color
sf.Color.Transparent = nil
--- @ingroup graphics
--- @brief Types of texture coordinates that can be used for rendering
---
--- @see `sf::RenderStates::coordinateType`
---@class sf.CoordinateType
--- Texture coordinates in range [0 .. 1]
---@field Normalized sf.CoordinateType
--- Texture coordinates in range [0 .. size]
---@field Pixels sf.CoordinateType
sf.CoordinateType = sf.CoordinateType or {}
--- @brief Class for loading, manipulating and saving images
---@class sf.Image
sf.Image = sf.Image or {}
--- @brief Construct the image and fill it with a unique color
---
--- @param size  Width and height of the image
--- @param color Fill color
---@overload fun(size: sf.Vector2u): sf.Image
---@overload fun(filename: string): sf.Image
---@overload fun(stream: sf.InputStream): sf.Image
---@overload fun(): sf.Image
---@overload fun(size: sf.Vector2u, pixels: any): sf.Image
---@overload fun(data: any): sf.Image
---@param size sf.Vector2u
---@param color sf.Color
---@return sf.Image
function sf.Image.new(size, color) end
--- @brief Resize the image and fill it with a unique color
---
--- @param size  Width and height of the image
--- @param color Fill color
---@overload fun(self: sf.Image, size: sf.Vector2u)
---@overload fun(self: sf.Image, size: sf.Vector2u, pixels: any)
---@param self sf.Image
---@param size sf.Vector2u
---@param color sf.Color
function sf.Image.resize(self, size, color) end
--- @brief Load the image from a file on disk
---
--- The supported image formats are bmp, png, tga, jpg, gif,
--- psd, hdr, pic and pnm. Some format options are not supported,
--- like jpeg with arithmetic coding or ASCII pnm.
--- If this function fails, the image is left unchanged.
---
--- @param filename Path of the image file to load
---
--- @return `true` if loading was successful
---
--- @see `loadFromMemory`, `loadFromStream`, `saveToFile`
---@type fun(self: sf.Image, filename: string): boolean
sf.Image.loadFromFile = function() end
--- @brief Load the image from a file in memory
---
--- The supported image formats are bmp, png, tga, jpg, gif,
--- psd, hdr, pic and pnm. Some format options are not supported,
--- like jpeg with arithmetic coding or ASCII pnm.
--- If this function fails, the image is left unchanged.
---
--- @param data Pointer to the file data in memory
--- @param size Size of the data to load, in bytes
---
--- @return `true` if loading was successful
---
--- @see `loadFromFile`, `loadFromStream`, `saveToMemory`
---@type fun(self: sf.Image, data: any): boolean
sf.Image.loadFromMemory = function() end
--- @brief Load the image from a custom stream
---
--- The supported image formats are bmp, png, tga, jpg, gif,
--- psd, hdr, pic and pnm. Some format options are not supported,
--- like jpeg with arithmetic coding or ASCII pnm.
--- If this function fails, the image is left unchanged.
---
--- @param stream Source stream to read from
---
--- @return `true` if loading was successful
---
--- @see `loadFromFile`, `loadFromMemory`
---@type fun(self: sf.Image, stream: sf.InputStream): boolean
sf.Image.loadFromStream = function() end
--- @brief Save the image to a file on disk
---
--- The format of the image is automatically deduced from
--- the extension. The supported image formats are bmp, png,
--- tga and jpg. The destination file is overwritten
--- if it already exists. This function fails if the image is empty.
---
--- @param filename Path of the file to save
---
--- @return `true` if saving was successful
---
--- @see `saveToMemory`, `loadFromFile`
---@type fun(self: sf.Image, filename: string): boolean
sf.Image.saveToFile = function() end
--- @brief Save the image to a buffer in memory
---
--- The format of the image must be specified.
--- The supported image formats are bmp, png, tga and jpg.
--- This function fails if the image is empty, or if
--- the format was invalid.
---
--- @param format Encoding format to use
---
--- @return Buffer with encoded data if saving was successful,
--- otherwise `std::nullopt`
---
--- @see `saveToFile`, `loadFromMemory`
---@type fun(self: sf.Image, format: string): integer[]|nil
sf.Image.saveToMemory = function() end
--- @brief Return the size (width and height) of the image
---
--- @return Size of the image, in pixels
---@type fun(self: sf.Image): sf.Vector2u
sf.Image.getSize = function() end
--- @brief Create a transparency mask from a specified color-key
---
--- This function sets the alpha value of every pixel matching
--- the given color to `alpha` (0 by default), so that they
--- become transparent.
---
--- @param color Color to make transparent
--- @param alpha Alpha value to assign to transparent pixels
---@overload fun(self: sf.Image, color: sf.Color)
---@param self sf.Image
---@param color sf.Color
---@param alpha integer
function sf.Image.createMaskFromColor(self, color, alpha) end
--- @brief Copy pixels from another image onto this one
---
--- This function does a slow pixel copy and should not be
--- used intensively. It can be used to prepare a complex
--- static image from several others, but if you need this
--- kind of feature in real-time you'd better use `sf::RenderTexture`.
---
--- If `sourceRect` is empty, the whole image is copied.
--- If `applyAlpha` is set to `true`, alpha blending is
--- applied from the source pixels to the destination pixels
--- using the @b over operator. If it is `false`, the source
--- pixels are copied unchanged with their alpha value.
---
--- See https://en.wikipedia.org/wiki/Alpha_compositing for
--- details on the @b over operator.
---
--- Note that this function can fail if either image is invalid
--- (i.e. zero-sized width or height), or if `sourceRect` is
--- not within the boundaries of the `source` parameter, or
--- if the destination area is out of the boundaries of this image.
---
--- On failure, the destination image is left unchanged.
---
--- @param source     Source image to copy
--- @param dest       Coordinates of the destination position
--- @param sourceRect Sub-rectangle of the source image to copy
--- @param applyAlpha Should the copy take into account the source transparency?
---
--- @return `true` if the operation was successful, `false` otherwise
---@overload fun(self: sf.Image, source: sf.Image, dest: sf.Vector2u, sourceRect: sf.IntRect): boolean
---@overload fun(self: sf.Image, source: sf.Image, dest: sf.Vector2u): boolean
---@param self sf.Image
---@param source sf.Image
---@param dest sf.Vector2u
---@param sourceRect sf.IntRect
---@param applyAlpha boolean
---@return boolean
function sf.Image.copy(self, source, dest, sourceRect, applyAlpha) end
--- @brief Change the color of a pixel
---
--- This function doesn't check the validity of the pixel
--- coordinates, using out-of-range values will result in
--- an undefined behavior.
---
--- @param coords Coordinates of pixel to change
--- @param color  New color of the pixel
---
--- @see `getPixel`
---@type fun(self: sf.Image, coords: sf.Vector2u, color: sf.Color)
sf.Image.setPixel = function() end
--- @brief Get the color of a pixel
---
--- This function doesn't check the validity of the pixel
--- coordinates, using out-of-range values will result in
--- an undefined behavior.
---
--- @param coords Coordinates of pixel to change
---
--- @return Color of the pixel at given coordinates
---
--- @see `setPixel`
---@type fun(self: sf.Image, coords: sf.Vector2u): sf.Color
sf.Image.getPixel = function() end
--- @brief Get a read-only pointer to the array of pixels
---
--- The returned value points to an array of RGBA pixels made of
--- 8 bit integer components. The size of the array is
--- `width * height * 4 (getSize().x * getSize().y * 4)`.
--- Warning: the returned pointer may become invalid if you
--- modify the image, so you should never store it for too long.
--- If the image is empty, a null pointer is returned.
---
--- @return Read-only pointer to the array of pixels
---@type fun(self: sf.Image): integer[]
sf.Image.getPixelsPtr = function() end
--- @brief Flip the image horizontally (left <-> right)
---@type fun(self: sf.Image)
sf.Image.flipHorizontally = function() end
--- @brief Flip the image vertically (top <-> bottom)
---@type fun(self: sf.Image)
sf.Image.flipVertically = function() end
--- @ingroup graphics
--- @brief Types of primitives that a `sf::VertexArray` can render
---
--- Points and lines have no area, therefore their thickness
--- will always be 1 pixel, regardless the current transform
--- and view.
---@class sf.PrimitiveType
--- List of individual points
---@field Points sf.PrimitiveType
--- List of individual lines
---@field Lines sf.PrimitiveType
--- List of connected lines, a point uses the previous point to form a line
---@field LineStrip sf.PrimitiveType
--- List of individual triangles
---@field Triangles sf.PrimitiveType
--- List of connected triangles, a point uses the two previous points to form a triangle
---@field TriangleStrip sf.PrimitiveType
--- List of connected triangles, a point uses the common center and the previous point to form a triangle
---@field TriangleFan sf.PrimitiveType
sf.PrimitiveType = sf.PrimitiveType or {}
--- @brief Utility class for manipulating 2D axis aligned rectangles
---@class sf.IntRect
--- Position of the top-left corner of the rectangle
---@field position sf.Vector2i
--- Size of the rectangle
---@field size sf.Vector2i
sf.IntRect = sf.IntRect or {}
--- @brief Construct the rectangle from position and size
---
--- Be careful, the last parameter is the size,
--- not the bottom-right corner!
---
--- @param position Position of the top-left corner of the rectangle
--- @param size     Size of the rectangle
---@overload fun(): sf.IntRect
---@overload fun(x: integer, y: integer, width: integer, height: integer): sf.IntRect
---@param position sf.Vector2i
---@param size sf.Vector2i
---@return sf.IntRect
function sf.IntRect.new(position, size) end
--- @brief Check if a point is inside the rectangle's area
---
--- This check is non-inclusive. If the point lies on the
--- edge of the rectangle, this function will return `false`.
---
--- @param point Point to test
---
--- @return `true` if the point is inside, `false` otherwise
---
--- @see `findIntersection`
---@type fun(self: sf.IntRect, point: sf.Vector2i): boolean
sf.IntRect.contains = function() end
--- @brief Check the intersection between two rectangles
---
--- @param rectangle Rectangle to test
---
--- @return Intersection rectangle if intersecting, `std::nullopt` otherwise
---
--- @see `contains`
---@type fun(self: sf.IntRect, rectangle: sf.IntRect): sf.IntRect|nil
sf.IntRect.findIntersection = function() end
--- @brief Get the position of the center of the rectangle
---
--- @return Center of rectangle
---@type fun(self: sf.IntRect): sf.Vector2i
sf.IntRect.getCenter = function() end

---@class sf.IntRect
---@operator eq(sf.IntRect): boolean
--- @brief Utility class for manipulating 2D axis aligned rectangles
---@class sf.FloatRect
--- Position of the top-left corner of the rectangle
---@field position sf.Vector2f
--- Size of the rectangle
---@field size sf.Vector2f
sf.FloatRect = sf.FloatRect or {}
--- @brief Construct the rectangle from position and size
---
--- Be careful, the last parameter is the size,
--- not the bottom-right corner!
---
--- @param position Position of the top-left corner of the rectangle
--- @param size     Size of the rectangle
---@overload fun(): sf.FloatRect
---@overload fun(x: number, y: number, width: number, height: number): sf.FloatRect
---@param position sf.Vector2f
---@param size sf.Vector2f
---@return sf.FloatRect
function sf.FloatRect.new(position, size) end
--- @brief Check if a point is inside the rectangle's area
---
--- This check is non-inclusive. If the point lies on the
--- edge of the rectangle, this function will return `false`.
---
--- @param point Point to test
---
--- @return `true` if the point is inside, `false` otherwise
---
--- @see `findIntersection`
---@type fun(self: sf.FloatRect, point: sf.Vector2f): boolean
sf.FloatRect.contains = function() end
--- @brief Check the intersection between two rectangles
---
--- @param rectangle Rectangle to test
---
--- @return Intersection rectangle if intersecting, `std::nullopt` otherwise
---
--- @see `contains`
---@type fun(self: sf.FloatRect, rectangle: sf.FloatRect): sf.FloatRect|nil
sf.FloatRect.findIntersection = function() end
--- @brief Get the position of the center of the rectangle
---
--- @return Center of rectangle
---@type fun(self: sf.FloatRect): sf.Vector2f
sf.FloatRect.getCenter = function() end

---@class sf.FloatRect
---@operator eq(sf.FloatRect): boolean
--- @brief Structure describing a glyph
---@class sf.Glyph
--- Offset to move horizontally to the next character
---@field advance number
--- Left offset after forced autohint. Internally used by getKerning()
---@field lsbDelta integer
--- Right offset after forced autohint. Internally used by getKerning()
---@field rsbDelta integer
--- Bounding rectangle of the glyph, in coordinates relative to the baseline
---@field bounds sf.FloatRect
--- Texture coordinates of the glyph inside the font's texture
---@field textureRect sf.IntRect
sf.Glyph = sf.Glyph or {}
---@type fun(): sf.Glyph
sf.Glyph.new = function() end
--- @brief Shader class (vertex, geometry and fragment)
---@class sf.Shader
sf.Shader = sf.Shader or {}
--- @brief Construct from vertex, geometry and fragment shader files
---
--- This constructor loads the vertex, geometry and fragment
--- shaders. If one of them fails to load, the shader is left
--- empty (the valid shader is unloaded).
--- The sources must be text files containing valid shaders
--- in GLSL language. GLSL is a C-like language dedicated to
--- OpenGL shaders; you'll probably need to read a good documentation
--- for it before writing your own shaders.
---
--- @param vertexShaderFilename   Path of the vertex shader file to load
--- @param geometryShaderFilename Path of the geometry shader file to load
--- @param fragmentShaderFilename Path of the fragment shader file to load
---
--- @throws sf::Exception if loading was unsuccessful
---
--- @see `loadFromFile`, `loadFromMemory`, `loadFromStream`
---@overload fun(vertexShaderStream: sf.InputStream, geometryShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader
---@overload fun(filename: string, type: sf.Shader.Type): sf.Shader
---@overload fun(vertexShaderFilename: string, fragmentShaderFilename: string): sf.Shader
---@overload fun(stream: sf.InputStream, type: sf.Shader.Type): sf.Shader
---@overload fun(vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): sf.Shader
---@overload fun(): sf.Shader
---@param vertexShaderFilename string
---@param geometryShaderFilename string
---@param fragmentShaderFilename string
---@return sf.Shader
function sf.Shader.new(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename) end
--- @brief Load the vertex, geometry and fragment shaders from files
---
--- This function loads the vertex, geometry and fragment
--- shaders. If one of them fails to load, the shader is left
--- empty (the valid shader is unloaded).
--- The sources must be text files containing valid shaders
--- in GLSL language. GLSL is a C-like language dedicated to
--- OpenGL shaders; you'll probably need to read a good documentation
--- for it before writing your own shaders.
---
--- @param vertexShaderFilename   Path of the vertex shader file to load
--- @param geometryShaderFilename Path of the geometry shader file to load
--- @param fragmentShaderFilename Path of the fragment shader file to load
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromMemory`, `loadFromStream`
---@overload fun(self: sf.Shader, filename: string, type: sf.Shader.Type): boolean
---@overload fun(self: sf.Shader, vertexShaderFilename: string, fragmentShaderFilename: string): boolean
---@param self sf.Shader
---@param vertexShaderFilename string
---@param geometryShaderFilename string
---@param fragmentShaderFilename string
---@return boolean
function sf.Shader.loadFromFile(self, vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename) end
--- @brief Load the vertex, geometry and fragment shaders from source codes in memory
---
--- This function loads the vertex, geometry and fragment
--- shaders. If one of them fails to load, the shader is left
--- empty (the valid shader is unloaded).
--- The sources must be valid shaders in GLSL language. GLSL is
--- a C-like language dedicated to OpenGL shaders; you'll
--- probably need to read a good documentation for it before
--- writing your own shaders.
---
--- @param vertexShader   String containing the source code of the vertex shader
--- @param geometryShader String containing the source code of the geometry shader
--- @param fragmentShader String containing the source code of the fragment shader
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromFile`, `loadFromStream`
---@overload fun(self: sf.Shader, shader: string, type: sf.Shader.Type): boolean
---@overload fun(self: sf.Shader, vertexShader: string, fragmentShader: string): boolean
---@param self sf.Shader
---@param vertexShader string
---@param geometryShader string
---@param fragmentShader string
---@return boolean
function sf.Shader.loadFromMemory(self, vertexShader, geometryShader, fragmentShader) end
--- @brief Load the vertex, geometry and fragment shaders from custom streams
---
--- This function loads the vertex, geometry and fragment
--- shaders. If one of them fails to load, the shader is left
--- empty (the valid shader is unloaded).
--- The source codes must be valid shaders in GLSL language.
--- GLSL is a C-like language dedicated to OpenGL shaders;
--- you'll probably need to read a good documentation for
--- it before writing your own shaders.
---
--- @param vertexShaderStream   Source stream to read the vertex shader from
--- @param geometryShaderStream Source stream to read the geometry shader from
--- @param fragmentShaderStream Source stream to read the fragment shader from
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromFile`, `loadFromMemory`
---@overload fun(self: sf.Shader, stream: sf.InputStream, type: sf.Shader.Type): boolean
---@overload fun(self: sf.Shader, vertexShaderStream: sf.InputStream, fragmentShaderStream: sf.InputStream): boolean
---@param self sf.Shader
---@param vertexShaderStream sf.InputStream
---@param geometryShaderStream sf.InputStream
---@param fragmentShaderStream sf.InputStream
---@return boolean
function sf.Shader.loadFromStream(self, vertexShaderStream, geometryShaderStream, fragmentShaderStream) end
--- @brief Specify value for @p float uniform
---
--- @param name Name of the uniform variable in GLSL
--- @param x    Value of the float scalar
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector2f)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector3f)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector4f)
---@overload fun(self: sf.Shader, name: string, x: integer)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector2i)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector3i)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector4i)
---@overload fun(self: sf.Shader, name: string, x: boolean)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector2b)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector3b)
---@overload fun(self: sf.Shader, name: string, vector: sf.Vector4b)
---@overload fun(self: sf.Shader, name: string, matrix: sf.Mat3)
---@overload fun(self: sf.Shader, name: string, matrix: sf.Mat4)
---@overload fun(self: sf.Shader, name: string, texture: sf.Texture)
---@overload fun(self: sf.Shader, name: string, arg1: sf.Shader.CurrentTextureType)
---@param self sf.Shader
---@param name string
---@param x number
function sf.Shader.setUniform(self, name, x) end
---@overload fun(self: sf.Shader, name: string, values: sf.Vector2f[])
---@overload fun(self: sf.Shader, name: string, values: sf.Vector3f[])
---@overload fun(self: sf.Shader, name: string, values: sf.Vector4f[])
---@overload fun(self: sf.Shader, name: string, values: sf.Mat3[])
---@overload fun(self: sf.Shader, name: string, values: sf.Mat4[])
---@param self sf.Shader
---@param name string
---@param values number[]
function sf.Shader.setUniformArray(self, name, values) end
---@type fun(self: sf.Shader, name: string, values: number[])
sf.Shader.setUniformFloatArray = function() end
---@type fun(self: sf.Shader, name: string, values: sf.Vector2f[])
sf.Shader.setUniformVec2Array = function() end
---@type fun(self: sf.Shader, name: string, values: sf.Vector3f[])
sf.Shader.setUniformVec3Array = function() end
---@type fun(self: sf.Shader, name: string, values: sf.Vector4f[])
sf.Shader.setUniformVec4Array = function() end
---@type fun(self: sf.Shader, name: string, values: sf.Mat3[])
sf.Shader.setUniformMat3Array = function() end
---@type fun(self: sf.Shader, name: string, values: sf.Mat4[])
sf.Shader.setUniformMat4Array = function() end
--- @brief Get the underlying OpenGL handle of the shader.
---
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return OpenGL handle of the shader or 0 if not yet loaded
---@type fun(self: sf.Shader): integer
sf.Shader.getNativeHandle = function() end
--- @brief Bind a shader for rendering
---
--- This function is not part of the graphics API, it mustn't be
--- used when drawing SFML entities. It must be used only if you
--- mix `sf::Shader` with OpenGL code.
---
--- @code
--- sf::Shader s1, s2;
--- ...
--- sf::Shader::bind(&s1);
--- // draw OpenGL stuff that use s1...
--- sf::Shader::bind(&s2);
--- // draw OpenGL stuff that use s2...
--- sf::Shader::bind(nullptr);
--- // draw OpenGL stuff that use no shader...
--- @endcode
---
--- @param shader Shader to bind, can be null to use no shader
---@type fun(shader: sf.Shader)
sf.Shader.bind = function() end
--- @brief Tell whether or not the system supports shaders
---
--- This function should always be called before using
--- the shader features. If it returns `false`, then
--- any attempt to use `sf::Shader` will fail.
---
--- @return `true` if shaders are supported, `false` otherwise
---@type fun(): boolean
sf.Shader.isAvailable = function() end
--- @brief Tell whether or not the system supports geometry shaders
---
--- This function should always be called before using
--- the geometry shader features. If it returns `false`, then
--- any attempt to use `sf::Shader` geometry shader features will fail.
---
--- This function can only return `true` if isAvailable() would also
--- return `true`, since shaders in general have to be supported in
--- order for geometry shaders to be supported as well.
---
--- Note: The first call to this function, whether by your
--- code or SFML will result in a context switch.
---
--- @return `true` if geometry shaders are supported, `false` otherwise
---@type fun(): boolean
sf.Shader.isGeometryAvailable = function() end
--- @brief Types of shaders
---@class sf.Shader.Type
--- %Vertex shader
---@field Vertex sf.Shader.Type
--- Geometry shader
---@field Geometry sf.Shader.Type
--- Fragment (pixel) shader
---@field Fragment sf.Shader.Type
sf.Shader.Type = sf.Shader.Type or {}
--- @brief Special type that can be passed to setUniform(),
--- and that represents the texture of the object being drawn
---
--- @see `setUniform(const std::string&, CurrentTextureType)`
---@class sf.Shader.CurrentTextureType
sf.Shader.CurrentTextureType = sf.Shader.CurrentTextureType or {}
---@type fun(): sf.Shader.CurrentTextureType
sf.Shader.CurrentTextureType.new = function() end
--- @brief Represents the texture of the object being drawn
---
--- @see `setUniform(const std::string&, CurrentTextureType)`
---@type sf.Shader.CurrentTextureType
sf.Shader.CurrentTexture = nil
--- @brief Enumeration of the stencil test comparisons that can be performed
---
--- The comparisons are mapped directly to their OpenGL equivalents,
--- specified by `glStencilFunc()`.
---@class sf.StencilComparison
--- The stencil test never passes
---@field Never sf.StencilComparison
--- The stencil test passes if the new value is less than the value in the stencil buffer
---@field Less sf.StencilComparison
--- The stencil test passes if the new value is less than or equal to the value in the stencil buffer
---@field LessEqual sf.StencilComparison
--- The stencil test passes if the new value is greater than the value in the stencil buffer
---@field Greater sf.StencilComparison
--- The stencil test passes if the new value is greater than or equal to the value in the stencil buffer
---@field GreaterEqual sf.StencilComparison
--- The stencil test passes if the new value is strictly equal to the value in the stencil buffer
---@field Equal sf.StencilComparison
--- The stencil test passes if the new value is strictly unequal to the value in the stencil buffer
---@field NotEqual sf.StencilComparison
--- The stencil test always passes
---@field Always sf.StencilComparison
sf.StencilComparison = sf.StencilComparison or {}
--- @brief Enumeration of the stencil buffer update operations
---
--- The update operations are mapped directly to their OpenGL equivalents,
--- specified by `glStencilOp()`.
---@class sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is not modified
---@field Keep sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is set to zero
---@field Zero sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is set to the new value
---@field Replace sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is incremented and if required clamped
---@field Increment sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is decremented and if required clamped
---@field Decrement sf.StencilUpdateOperation
--- If the stencil test passes, the value in the stencil buffer is bitwise inverted
---@field Invert sf.StencilUpdateOperation
sf.StencilUpdateOperation = sf.StencilUpdateOperation or {}
--- @brief Stencil value type (also used as a mask)
---@class sf.StencilValue
--- The stored stencil value
---@field value integer
sf.StencilValue = sf.StencilValue or {}
--- @brief Construct a stencil value from a signed integer
---
--- @param theValue Signed integer value to use
---@overload fun(theValue: integer): sf.StencilValue
---@param theValue integer
---@return sf.StencilValue
function sf.StencilValue.new(theValue) end
--- @brief Stencil modes for drawing
---@class sf.StencilMode
--- The comparison we're performing the stencil test with
---@field stencilComparison sf.StencilComparison
---@field stencilUpdateOperation sf.StencilUpdateOperation
--- The reference value we're performing the stencil test with
---@field stencilReference sf.StencilValue
--- The mask to apply to both the reference value and the value in the stencil buffer
---@field stencilMask sf.StencilValue
--- Whether we should update the color buffer in addition to the stencil buffer
---@field stencilOnly boolean
sf.StencilMode = sf.StencilMode or {}
---@type fun(): sf.StencilMode
sf.StencilMode.new = function() end
--- @brief Image living on the graphics card that can be used for drawing
---@class sf.Texture
sf.Texture = sf.Texture or {}
--- @brief Construct the texture from a sub-rectangle of a file on disk
---
--- The `area` argument can be used to load only a sub-rectangle
--- of the whole image. If you want the entire image then leave
--- the default value (which is an empty `IntRect`).
--- If the `area` rectangle crosses the bounds of the image, it
--- is adjusted to fit the image size.
---
--- The maximum size for a texture depends on the graphics
--- driver and can be retrieved with the `getMaximumSize` function.
---
--- @param filename Path of the image file to load
--- @param sRgb     `true` to enable sRGB conversion, `false` to disable it
--- @param area     Area of the image to load
---
--- @throws sf::Exception if loading was unsuccessful
---
--- @see `loadFromFile`, `loadFromMemory`, `loadFromStream`, `loadFromImage`
---@overload fun(stream: sf.InputStream, sRgb: boolean, area: sf.IntRect): sf.Texture
---@overload fun(image: sf.Image, sRgb: boolean, area: sf.IntRect): sf.Texture
---@overload fun(filename: string, sRgb: boolean): sf.Texture
---@overload fun(stream: sf.InputStream, sRgb: boolean): sf.Texture
---@overload fun(image: sf.Image, sRgb: boolean): sf.Texture
---@overload fun(size: sf.Vector2u, sRgb: boolean): sf.Texture
---@overload fun(filename: string): sf.Texture
---@overload fun(stream: sf.InputStream): sf.Texture
---@overload fun(image: sf.Image): sf.Texture
---@overload fun(size: sf.Vector2u): sf.Texture
---@overload fun(): sf.Texture
---@overload fun(data: any, sRgb: boolean, area: sf.IntRect): sf.Texture
---@overload fun(data: any, sRgb: boolean): sf.Texture
---@overload fun(data: any): sf.Texture
---@param filename string
---@param sRgb boolean
---@param area sf.IntRect
---@return sf.Texture
function sf.Texture.new(filename, sRgb, area) end
--- @brief Resize the texture
---
--- If this function fails, the texture is left unchanged.
---
--- @param size Width and height of the texture
--- @param sRgb `true` to enable sRGB conversion, `false` to disable it
---
--- @return `true` if resizing was successful, `false` if it failed
---@overload fun(self: sf.Texture, size: sf.Vector2u): boolean
---@param self sf.Texture
---@param size sf.Vector2u
---@param sRgb boolean
---@return boolean
function sf.Texture.resize(self, size, sRgb) end
--- @brief Load the texture from a file on disk
---
--- The `area` argument can be used to load only a sub-rectangle
--- of the whole image. If you want the entire image then leave
--- the default value (which is an empty `IntRect`).
--- If the `area` rectangle crosses the bounds of the image, it
--- is adjusted to fit the image size.
---
--- The maximum size for a texture depends on the graphics
--- driver and can be retrieved with the `getMaximumSize` function.
---
--- If this function fails, the texture is left unchanged.
---
--- @param filename Path of the image file to load
--- @param sRgb     `true` to enable sRGB conversion, `false` to disable it
--- @param area     Area of the image to load
---
--- @return `true` if loading was successful, `false` if it failed
---
--- @see `loadFromMemory`, `loadFromStream`, `loadFromImage`
---@overload fun(self: sf.Texture, filename: string, sRgb: boolean): boolean
---@overload fun(self: sf.Texture, filename: string): boolean
---@param self sf.Texture
---@param filename string
---@param sRgb boolean
---@param area sf.IntRect
---@return boolean
function sf.Texture.loadFromFile(self, filename, sRgb, area) end
--- @brief Load the texture from a file in memory
---
--- The `area` argument can be used to load only a sub-rectangle
--- of the whole image. If you want the entire image then leave
--- the default value (which is an empty `IntRect`).
--- If the `area` rectangle crosses the bounds of the image, it
--- is adjusted to fit the image size.
---
--- The maximum size for a texture depends on the graphics
--- driver and can be retrieved with the `getMaximumSize` function.
---
--- If this function fails, the texture is left unchanged.
---
--- @param data Pointer to the file data in memory
--- @param size Size of the data to load, in bytes
--- @param sRgb `true` to enable sRGB conversion, `false` to disable it
--- @param area Area of the image to load
---
--- @return `true` if loading was successful, `false` if it failed
---
--- @see `loadFromFile`, `loadFromStream`, `loadFromImage`
---@overload fun(self: sf.Texture, data: any, sRgb: boolean): boolean
---@overload fun(self: sf.Texture, data: any): boolean
---@param self sf.Texture
---@param data any
---@param sRgb boolean
---@param area sf.IntRect
---@return boolean
function sf.Texture.loadFromMemory(self, data, sRgb, area) end
--- @brief Load the texture from a custom stream
---
--- The `area` argument can be used to load only a sub-rectangle
--- of the whole image. If you want the entire image then leave
--- the default value (which is an empty `IntRect`).
--- If the `area` rectangle crosses the bounds of the image, it
--- is adjusted to fit the image size.
---
--- The maximum size for a texture depends on the graphics
--- driver and can be retrieved with the `getMaximumSize` function.
---
--- If this function fails, the texture is left unchanged.
---
--- @param stream Source stream to read from
--- @param sRgb   `true` to enable sRGB conversion, `false` to disable it
--- @param area   Area of the image to load
---
--- @return `true` if loading was successful, `false` if it failed
---
--- @see `loadFromFile`, `loadFromMemory`, `loadFromImage`
---@overload fun(self: sf.Texture, stream: sf.InputStream, sRgb: boolean): boolean
---@overload fun(self: sf.Texture, stream: sf.InputStream): boolean
---@param self sf.Texture
---@param stream sf.InputStream
---@param sRgb boolean
---@param area sf.IntRect
---@return boolean
function sf.Texture.loadFromStream(self, stream, sRgb, area) end
--- @brief Load the texture from an image
---
--- The `area` argument can be used to load only a sub-rectangle
--- of the whole image. If you want the entire image then leave
--- the default value (which is an empty `IntRect`).
--- If the `area` rectangle crosses the bounds of the image, it
--- is adjusted to fit the image size.
---
--- The maximum size for a texture depends on the graphics
--- driver and can be retrieved with the `getMaximumSize` function.
---
--- If this function fails, the texture is left unchanged.
---
--- @param image Image to load into the texture
--- @param sRgb  `true` to enable sRGB conversion, `false` to disable it
--- @param area  Area of the image to load
---
--- @return `true` if loading was successful, `false` if it failed
---
--- @see `loadFromFile`, `loadFromMemory`
---@overload fun(self: sf.Texture, image: sf.Image, sRgb: boolean): boolean
---@overload fun(self: sf.Texture, image: sf.Image): boolean
---@param self sf.Texture
---@param image sf.Image
---@param sRgb boolean
---@param area sf.IntRect
---@return boolean
function sf.Texture.loadFromImage(self, image, sRgb, area) end
--- @brief Return the size of the texture
---
--- @return Size in pixels
---@type fun(self: sf.Texture): sf.Vector2u
sf.Texture.getSize = function() end
--- @brief Copy the texture pixels to an image
---
--- This function performs a slow operation that downloads
--- the texture's pixels from the graphics card and copies
--- them to a new image, potentially applying transformations
--- to pixels if necessary (texture may be padded or flipped).
---
--- @return Image containing the texture's pixels
---
--- @see `loadFromImage`
---@type fun(self: sf.Texture): sf.Image
sf.Texture.copyToImage = function() end
--- @brief Update a part of this texture from another texture
---
--- No additional check is performed on the size of the texture.
--- Passing an invalid combination of texture size and destination
--- will lead to an undefined behavior.
---
--- This function does nothing if either texture was not
--- previously created.
---
--- @param texture Source texture to copy to this texture
--- @param dest    Coordinates of the destination position
---@overload fun(self: sf.Texture, image: sf.Image, dest: sf.Vector2u)
---@overload fun(self: sf.Texture, window: sf.Window, dest: sf.Vector2u)
---@overload fun(self: sf.Texture, texture: sf.Texture)
---@overload fun(self: sf.Texture, image: sf.Image)
---@overload fun(self: sf.Texture, window: sf.Window)
---@overload fun(self: sf.Texture, pixels: any, size: sf.Vector2u, dest: sf.Vector2u)
---@overload fun(self: sf.Texture, pixels: any)
---@param self sf.Texture
---@param texture sf.Texture
---@param dest sf.Vector2u
function sf.Texture.update(self, texture, dest) end
--- @brief Enable or disable the smooth filter
---
--- When the filter is activated, the texture appears smoother
--- so that pixels are less noticeable. However if you want
--- the texture to look exactly the same as its source file,
--- you should leave it disabled.
--- The smooth filter is disabled by default.
---
--- @param smooth `true` to enable smoothing, `false` to disable it
---
--- @see `isSmooth`
---@type fun(self: sf.Texture, smooth: boolean)
sf.Texture.setSmooth = function() end
--- @brief Tell whether the smooth filter is enabled or not
---
--- @return `true` if smoothing is enabled, `false` if it is disabled
---
--- @see `setSmooth`
---@type fun(self: sf.Texture): boolean
sf.Texture.isSmooth = function() end
--- @brief Tell whether the texture source is converted from sRGB or not
---
--- @return `true` if the texture source is converted from sRGB, `false` if not
---
--- @see `setSrgb`
---@type fun(self: sf.Texture): boolean
sf.Texture.isSrgb = function() end
--- @brief Enable or disable repeating
---
--- Repeating is involved when using texture coordinates
--- outside the texture rectangle [0, 0, width, height].
--- In this case, if repeat mode is enabled, the whole texture
--- will be repeated as many times as needed to reach the
--- coordinate (for example, if the X texture coordinate is
--- 3 * width, the texture will be repeated 3 times).
--- If repeat mode is disabled, the "extra space" will instead
--- be filled with border pixels.
--- Warning: on very old graphics cards, white pixels may appear
--- when the texture is repeated. With such cards, repeat mode
--- can be used reliably only if the texture has power-of-two
--- dimensions (such as 256x128).
--- Repeating is disabled by default.
---
--- @param repeated `true` to repeat the texture, `false` to disable repeating
---
--- @see `isRepeated`
---@type fun(self: sf.Texture, repeated: boolean)
sf.Texture.setRepeated = function() end
--- @brief Tell whether the texture is repeated or not
---
--- @return `true` if repeat mode is enabled, `false` if it is disabled
---
--- @see `setRepeated`
---@type fun(self: sf.Texture): boolean
sf.Texture.isRepeated = function() end
--- @brief Generate a mipmap using the current texture data
---
--- Mipmaps are pre-computed chains of optimized textures. Each
--- level of texture in a mipmap is generated by halving each of
--- the previous level's dimensions. This is done until the final
--- level has the size of 1x1. The textures generated in this process may
--- make use of more advanced filters which might improve the visual quality
--- of textures when they are applied to objects much smaller than they are.
--- This is known as minification. Because fewer texels (texture elements)
--- have to be sampled from when heavily minified, usage of mipmaps
--- can also improve rendering performance in certain scenarios.
---
--- Mipmap generation relies on the necessary OpenGL extension being
--- available. If it is unavailable or generation fails due to another
--- reason, this function will return `false`. Mipmap data is only valid from
--- the time it is generated until the next time the base level image is
--- modified, at which point this function will have to be called again to
--- regenerate it.
---
--- @return `true` if mipmap generation was successful, `false` if unsuccessful
---@type fun(self: sf.Texture): boolean
sf.Texture.generateMipmap = function() end
--- @brief Swap the contents of this texture with those of another
---
--- @param right Instance to swap with
---@type fun(self: sf.Texture, right: sf.Texture)
sf.Texture.swap = function() end
--- @brief Get the underlying OpenGL handle of the texture.
---
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return OpenGL handle of the texture or 0 if not yet created
---@type fun(self: sf.Texture): integer
sf.Texture.getNativeHandle = function() end
--- @brief Bind a texture for rendering
---
--- This function is not part of the graphics API, it mustn't be
--- used when drawing SFML entities. It must be used only if you
--- mix `sf::Texture` with OpenGL code.
--- It only changes the `GL_TEXTURE_2D` binding. Direct OpenGL
--- rendering code is responsible for converting texture coordinates
--- in its shader when pixel coordinates are used.
---
--- @code
--- sf::Texture t1, t2;
--- ...
--- sf::Texture::bind(&t1);
--- // draw OpenGL stuff that use t1...
--- sf::Texture::bind(&t2);
--- // draw OpenGL stuff that use t2...
--- sf::Texture::bind(nullptr);
--- // draw OpenGL stuff that use no texture...
--- @endcode
---
--- @param texture Pointer to the texture to bind, can be null to use no texture
---@type fun(texture: sf.Texture)
sf.Texture.bind = function() end
--- @brief Get the maximum texture size allowed
---
--- This maximum size is defined by the graphics driver.
--- You can expect a value of 512 pixels for low-end graphics
--- card, and up to 8192 pixels or more for newer hardware.
---
--- @return Maximum size allowed for textures, in pixels
---@type fun(): integer
sf.Texture.getMaximumSize = function() end
--- @brief Swap the contents of one texture with those of another
---
--- @param left First instance to swap
--- @param right Second instance to swap
---@type fun(left: sf.Texture, right: sf.Texture)
sf.swap = function() end
--- @brief Class for loading and manipulating character fonts
---@class sf.Font
sf.Font = sf.Font or {}
--- @brief Construct the font from a file
---
--- The supported font formats are: TrueType, Type 1, CFF,
--- OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
--- Note that this function knows nothing about the standard
--- fonts installed on the user's system, thus you can't
--- load them directly.
---
--- @warning SFML cannot preload all the font data in this
--- function, so the file has to remain accessible until
--- the `sf::Font` object opens a new font or is destroyed.
---
--- @param filename Path of the font file to open
---
--- @throws sf::Exception if opening was unsuccessful
---
--- @see `openFromFile`, `openFromMemory`, `openFromStream`
---@overload fun(stream: sf.InputStream): sf.Font
---@overload fun(): sf.Font
---@overload fun(data: any): sf.Font
---@param filename string
---@return sf.Font
function sf.Font.new(filename) end
--- @brief Open the font from a file
---
--- The supported font formats are: TrueType, Type 1, CFF,
--- OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
--- Note that this function knows nothing about the standard
--- fonts installed on the user's system, thus you can't
--- load them directly.
---
--- @warning SFML cannot preload all the font data in this
--- function, so the file has to remain accessible until
--- the `sf::Font` object opens a new font or is destroyed.
---
--- @param filename Path of the font file to load
---
--- @return `true` if opening succeeded, `false` if it failed
---
--- @see `openFromMemory`, `openFromStream`
---@type fun(self: sf.Font, filename: string): boolean
sf.Font.openFromFile = function() end
--- @brief Open the font from a file in memory
---
--- The supported font formats are: TrueType, Type 1, CFF,
--- OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
---
--- @warning SFML cannot preload all the font data in this
--- function, so the buffer pointed by `data` has to remain
--- valid until the `sf::Font` object opens a new font or
--- is destroyed.
---
--- @param data        Pointer to the file data in memory
--- @param sizeInBytes Size of the data to load, in bytes
---
--- @return `true` if opening succeeded, `false` if it failed
---
--- @see `openFromFile`, `openFromStream`
---@type fun(self: sf.Font, data: any): boolean
sf.Font.openFromMemory = function() end
--- @brief Open the font from a custom stream
---
--- The supported font formats are: TrueType, Type 1, CFF,
--- OpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.
---
--- @warning SFML cannot preload all the font data in this
--- function, so the stream has to remain accessible until
--- the `sf::Font` object opens a new font or is destroyed.
---
--- @param stream Source stream to read from
---
--- @return `true` if opening succeeded, `false` if it failed
---
--- @see `openFromFile`, `openFromMemory`
---@type fun(self: sf.Font, stream: sf.InputStream): boolean
sf.Font.openFromStream = function() end
--- @brief Get the font information
---
--- @return A structure that holds the font information
---@type fun(self: sf.Font): sf.Font.Info
sf.Font.getInfo = function() end
--- @brief Retrieve a glyph of the font by glyph ID
---
--- If the font is a bitmap font, not all character sizes
--- might be available. If the glyph is not available at the
--- requested size, an empty glyph is returned.
---
--- This function is only useful for getting the glyphs
--- returned in the data from calling `shape`.
---
--- Be aware that using a negative value for the outline
--- thickness will cause distorted rendering.
---
--- @param id               ID of the glyph to get
--- @param characterSize    Reference character size
--- @param bold             Retrieve the bold version or the regular one?
--- @param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)
---
--- @return The glyph corresponding to `id` and `characterSize`
---@overload fun(self: sf.Font, id: integer, characterSize: integer, bold: boolean): sf.Glyph
---@param self sf.Font
---@param id integer
---@param characterSize integer
---@param bold boolean
---@param outlineThickness number
---@return sf.Glyph
function sf.Font.getGlyphById(self, id, characterSize, bold, outlineThickness) end
--- @brief Retrieve a glyph of the font
---
--- If the font is a bitmap font, not all character sizes
--- might be available. If the glyph is not available at the
--- requested size, an empty glyph is returned.
---
--- You may want to use `hasGlyph` to determine if the
--- glyph exists before requesting it. If the glyph does not
--- exist, a font specific default is returned.
---
--- Be aware that using a negative value for the outline
--- thickness will cause distorted rendering.
---
--- @param codePoint        Unicode code point of the character to get
--- @param characterSize    Reference character size
--- @param bold             Retrieve the bold version or the regular one?
--- @param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)
---
--- @return The glyph corresponding to `codePoint` and `characterSize`
---@overload fun(self: sf.Font, codePoint: integer, characterSize: integer, bold: boolean): sf.Glyph
---@param self sf.Font
---@param codePoint integer
---@param characterSize integer
---@param bold boolean
---@param outlineThickness number
---@return sf.Glyph
function sf.Font.getGlyph(self, codePoint, characterSize, bold, outlineThickness) end
--- @brief Determine if this font has a glyph representing the requested code point
---
--- Most fonts only include a very limited selection of glyphs from
--- specific Unicode subsets, like Latin, Cyrillic, or Asian characters.
---
--- While code points without representation will return a font specific
--- default character, it might be useful to verify whether specific
--- code points are included to determine whether a font is suited
--- to display text in a specific language.
---
--- @param codePoint Unicode code point to check
---
--- @return `true` if the codepoint has a glyph representation, `false` otherwise
---@type fun(self: sf.Font, codePoint: integer): boolean
sf.Font.hasGlyph = function() end
--- @brief Get the kerning offset of two glyphs
---
--- @deprecated Use the `getKerning(char32_t, char32_t, unsigned int, bool)` overload instead.
---
--- The kerning is an extra offset (negative) to apply between two
--- glyphs when rendering them, to make the pair look more "natural".
--- For example, the pair "AV" have a special kerning to make them
--- closer than other characters. Most of the glyphs pairs have a
--- kerning offset of zero, though.
---
--- @param first         Unicode code point of the first character
--- @param second        Unicode code point of the second character
--- @param characterSize Reference character size
--- @param bold          Retrieve the bold version or the regular one?
---
--- @return Kerning value for `first` and `second`, in pixels
---@overload fun(self: sf.Font, first: integer, second: integer, characterSize: integer, bold: boolean): number
---@overload fun(self: sf.Font, first: integer, second: integer, characterSize: integer): number
---@overload fun(self: sf.Font, first: integer, second: integer, characterSize: integer): number
---@param self sf.Font
---@param first integer
---@param second integer
---@param characterSize integer
---@param bold boolean
---@return number
function sf.Font.getKerning(self, first, second, characterSize, bold) end
--- @brief Get the ascent
---
--- The ascent is the largest distance between the baseline and
--- the top of all glyphs in the font.
---
--- Be aware that there is no uniform definition of how the
--- ascent is calculated. It can vary from font to font.
---
--- @param characterSize Reference character size
---
--- @return Ascent, in pixels
---@type fun(self: sf.Font, characterSize: integer): number
sf.Font.getAscent = function() end
--- @brief Get the descent
---
--- The descent is the largest distance between the baseline and
--- the bottom of all glyphs in the font.
---
--- Be aware that there is no uniform definition of how the
--- descent is calculated. It can vary from font to font.
---
--- The descent shares the same coordinate system as the
--- ascent. This means that it will be negative for distances
--- below the baseline.
---
--- @param characterSize Reference character size
---
--- @return Descent, in pixels
---@type fun(self: sf.Font, characterSize: integer): number
sf.Font.getDescent = function() end
--- @brief Get the line spacing
---
--- Line spacing is the vertical offset to apply between two
--- consecutive lines of text.
---
--- @param characterSize Reference character size
---
--- @return Line spacing, in pixels
---@type fun(self: sf.Font, characterSize: integer): number
sf.Font.getLineSpacing = function() end
--- @brief Get the position of the underline
---
--- Underline position is the vertical offset to apply between the
--- baseline and the underline.
---
--- @param characterSize Reference character size
---
--- @return Underline position, in pixels
---
--- @see `getUnderlineThickness`
---@type fun(self: sf.Font, characterSize: integer): number
sf.Font.getUnderlinePosition = function() end
--- @brief Get the thickness of the underline
---
--- Underline thickness is the vertical size of the underline.
---
--- @param characterSize Reference character size
---
--- @return Underline thickness, in pixels
---
--- @see `getUnderlinePosition`
---@type fun(self: sf.Font, characterSize: integer): number
sf.Font.getUnderlineThickness = function() end
--- @brief Retrieve the texture containing the loaded glyphs of a certain size
---
--- The contents of the returned texture changes as more glyphs
--- are requested, thus it is not very relevant. It is mainly
--- used internally by `sf::Text`.
---
--- @param characterSize Reference character size
---
--- @return Texture containing the glyphs of the requested size
---@type fun(self: sf.Font, characterSize: integer): sf.Texture
sf.Font.getTexture = function() end
--- @brief Enable or disable the smooth filter
---
--- When the filter is activated, the font appears smoother
--- so that pixels are less noticeable. However if you want
--- the font to look exactly the same as its source file,
--- you should disable it.
--- The smooth filter is enabled by default.
---
--- @param smooth `true` to enable smoothing, `false` to disable it
---
--- @see `isSmooth`
---@type fun(self: sf.Font, smooth: boolean)
sf.Font.setSmooth = function() end
--- @brief Tell whether the smooth filter is enabled or not
---
--- @return `true` if smoothing is enabled, `false` if it is disabled
---
--- @see `setSmooth`
---@type fun(self: sf.Font): boolean
sf.Font.isSmooth = function() end
--- @brief Holds various information about a font
---@class sf.Font.Info
--- A unique ID that identifies the font
---@field id integer
--- The font family
---@field family string
--- Has kerning information
---@field hasKerning boolean
--- Has native vertical metrics
---@field hasVerticalMetrics boolean
sf.Font.Info = sf.Font.Info or {}
---@type fun(): sf.Font.Info
sf.Font.Info.new = function() end
--- @brief 3x3 transform matrix
---@class sf.Transform
sf.Transform = sf.Transform or {}
--- @brief Construct a transform from a 3x3 matrix
---
--- @param a00 Element (0, 0) of the matrix
--- @param a01 Element (0, 1) of the matrix
--- @param a02 Element (0, 2) of the matrix
--- @param a10 Element (1, 0) of the matrix
--- @param a11 Element (1, 1) of the matrix
--- @param a12 Element (1, 2) of the matrix
--- @param a20 Element (2, 0) of the matrix
--- @param a21 Element (2, 1) of the matrix
--- @param a22 Element (2, 2) of the matrix
---@overload fun(): sf.Transform
---@param a00 number
---@param a01 number
---@param a02 number
---@param a10 number
---@param a11 number
---@param a12 number
---@param a20 number
---@param a21 number
---@param a22 number
---@return sf.Transform
function sf.Transform.new(a00, a01, a02, a10, a11, a12, a20, a21, a22) end
--- @brief Return the transform as a 4x4 matrix
---
--- This function returns a pointer to an array of 16 floats
--- containing the transform elements as a 4x4 matrix, which
--- is directly compatible with OpenGL functions.
---
--- @code
--- sf::Transform transform = ...;
--- glUniformMatrix4fv(location, 1, GL_FALSE, transform.getMatrix());
--- @endcode
---
--- @return Pointer to a 4x4 matrix
---@type fun(self: sf.Transform): number[]
sf.Transform.getMatrix = function() end
--- @brief Return the inverse of the transform
---
--- If the inverse cannot be computed, an identity transform
--- is returned.
---
--- @return A new transform which is the inverse of self
---@type fun(self: sf.Transform): sf.Transform
sf.Transform.getInverse = function() end
--- @brief Transform a 2D point
---
--- These two statements are equivalent:
--- @code
--- sf::Vector2f transformedPoint = matrix.transformPoint(point);
--- sf::Vector2f transformedPoint = matrix * point;
--- @endcode
---
--- @param point Point to transform
---
--- @return Transformed point
---@type fun(self: sf.Transform, point: sf.Vector2f): sf.Vector2f
sf.Transform.transformPoint = function() end
--- @brief Transform a rectangle
---
--- Since SFML doesn't provide support for oriented rectangles,
--- the result of this function is always an axis-aligned
--- rectangle. Which means that if the transform contains a
--- rotation, the bounding rectangle of the transformed rectangle
--- is returned.
---
--- @param rectangle Rectangle to transform
---
--- @return Transformed rectangle
---@type fun(self: sf.Transform, rectangle: sf.FloatRect): sf.FloatRect
sf.Transform.transformRect = function() end
--- @brief Combine the current transform with another one
---
--- The result is a transform that is equivalent to applying
--- `transform` followed by `*this`. Mathematically, it is
--- equivalent to a matrix multiplication `(*this) * transform`.
---
--- These two statements are equivalent:
--- @code
--- left.combine(right);
--- left *= right;
--- @endcode
---
--- @param transform Transform to combine with this transform
---
--- @return Reference to `*this`
---@type fun(self: sf.Transform, transform: sf.Transform): sf.Transform
sf.Transform.combine = function() end
--- @brief Combine the current transform with a translation
---
--- This function returns a reference to `*this`, so that calls
--- can be chained.
--- @code
--- sf::Transform transform;
--- transform.translate(sf::Vector2f(100, 200)).rotate(sf::degrees(45));
--- @endcode
---
--- @param offset Translation offset to apply
---
--- @return Reference to `*this`
---
--- @see `rotate`, `scale`
---@type fun(self: sf.Transform, offset: sf.Vector2f): sf.Transform
sf.Transform.translate = function() end
--- @brief Combine the current transform with a rotation
---
--- The center of rotation is provided for convenience as a second
--- argument, so that you can build rotations around arbitrary points
--- more easily (and efficiently) than the usual
--- `translate(-center).rotate(angle).translate(center)`.
---
--- This function returns a reference to `*this`, so that calls
--- can be chained.
--- @code
--- sf::Transform transform;
--- transform.rotate(sf::degrees(90), sf::Vector2f(8, 3)).translate(sf::Vector2f(50, 20));
--- @endcode
---
--- @param angle Rotation angle
--- @param center Center of rotation
---
--- @return Reference to `*this`
---
--- @see `translate`, `scale`
---@overload fun(self: sf.Transform, angle: sf.Angle): sf.Transform
---@param self sf.Transform
---@param angle sf.Angle
---@param center sf.Vector2f
---@return sf.Transform
function sf.Transform.rotate(self, angle, center) end
--- @brief Combine the current transform with a scaling
---
--- The center of scaling is provided for convenience as a second
--- argument, so that you can build scaling around arbitrary points
--- more easily (and efficiently) than the usual
--- `translate(-center).scale(factors).translate(center)`.
---
--- This function returns a reference to `*this`, so that calls
--- can be chained.
--- @code
--- sf::Transform transform;
--- transform.scale(sf::Vector2f(2, 1), sf::Vector2f(8, 3)).rotate(45);
--- @endcode
---
--- @param factors Scaling factors
--- @param center Center of scaling
---
--- @return Reference to `*this`
---
--- @see `translate`, `rotate`
---@overload fun(self: sf.Transform, factors: sf.Vector2f): sf.Transform
---@param self sf.Transform
---@param factors sf.Vector2f
---@param center sf.Vector2f
---@return sf.Transform
function sf.Transform.scale(self, factors, center) end
--- The identity transform (does nothing)
---@type sf.Transform
sf.Transform.Identity = nil
---@type sf.Vector2f
sf.Vec2 = nil
---@type sf.Vector2i
sf.Ivec2 = nil
--- @brief Class template for manipulating
--- 2-dimensional vectors
---@class sf.Vector2b
--- X coordinate of the vector
---@field x boolean
--- Y coordinate of the vector
---@field y boolean
sf.Vector2b = sf.Vector2b or {}
--- @brief Construct the vector from cartesian coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
---@overload fun(): sf.Vector2b
---@param x boolean
---@param y boolean
---@return sf.Vector2b
function sf.Vector2b.new(x, y) end
---@type fun(self: sf.Vector2b): boolean, boolean
sf.Vector2b.unpack = function() end

---@class sf.Vector2b
---@operator eq(sf.Vector2b): boolean
---@type sf.Vector2b
sf.Bvec2 = nil
---@type sf.Vector3f
sf.Vec3 = nil
---@type sf.Vector3i
sf.Ivec3 = nil
--- @brief Utility template class for manipulating
--- 3-dimensional vectors
---@class sf.Vector3b
--- X coordinate of the vector
---@field x boolean
--- Y coordinate of the vector
---@field y boolean
--- Z coordinate of the vector
---@field z boolean
sf.Vector3b = sf.Vector3b or {}
--- @brief Construct the vector from its coordinates
---
--- @param x X coordinate
--- @param y Y coordinate
--- @param z Z coordinate
---@overload fun(): sf.Vector3b
---@param x boolean
---@param y boolean
---@param z boolean
---@return sf.Vector3b
function sf.Vector3b.new(x, y, z) end
---@type fun(self: sf.Vector3b): boolean, boolean, boolean
sf.Vector3b.unpack = function() end

---@class sf.Vector3b
---@operator eq(sf.Vector3b): boolean
---@type sf.Vector3b
sf.Bvec3 = nil
--- @brief 4D vector type, used to set uniforms in GLSL
---@class sf.Vector4f
--- 1st component (X) of the 4D vector
---@field x number
--- 2nd component (Y) of the 4D vector
---@field y number
--- 3rd component (Z) of the 4D vector
---@field z number
--- 4th component (W) of the 4D vector
---@field w number
sf.Vector4f = sf.Vector4f or {}
--- @brief Construct vector implicitly from color
---
--- Vector is normalized to [0, 1] for floats, and left as-is
--- for ints. Not defined for other template arguments.
---
--- @param color Color instance
---@overload fun(color: sf.Color): sf.Vector4f
---@overload fun(): sf.Vector4f
---@param x number
---@param y number
---@param z number
---@param w number
---@return sf.Vector4f
function sf.Vector4f.new(x, y, z, w) end
---@type fun(self: sf.Vector4f): number, number, number, number
sf.Vector4f.unpack = function() end
---@type sf.Vector4f
sf.Vec4 = nil
--- @brief 4D vector type, used to set uniforms in GLSL
---@class sf.Vector4i
--- 1st component (X) of the 4D vector
---@field x integer
--- 2nd component (Y) of the 4D vector
---@field y integer
--- 3rd component (Z) of the 4D vector
---@field z integer
--- 4th component (W) of the 4D vector
---@field w integer
sf.Vector4i = sf.Vector4i or {}
--- @brief Construct vector implicitly from color
---
--- Vector is normalized to [0, 1] for floats, and left as-is
--- for ints. Not defined for other template arguments.
---
--- @param color Color instance
---@overload fun(color: sf.Color): sf.Vector4i
---@overload fun(): sf.Vector4i
---@param x integer
---@param y integer
---@param z integer
---@param w integer
---@return sf.Vector4i
function sf.Vector4i.new(x, y, z, w) end
---@type fun(self: sf.Vector4i): integer, integer, integer, integer
sf.Vector4i.unpack = function() end
---@type sf.Vector4i
sf.Ivec4 = nil
--- @brief 4D vector type, used to set uniforms in GLSL
---@class sf.Vector4b
--- 1st component (X) of the 4D vector
---@field x boolean
--- 2nd component (Y) of the 4D vector
---@field y boolean
--- 3rd component (Z) of the 4D vector
---@field z boolean
--- 4th component (W) of the 4D vector
---@field w boolean
sf.Vector4b = sf.Vector4b or {}
--- @brief Default constructor, creates a zero vector
---@overload fun(): sf.Vector4b
---@param x boolean
---@param y boolean
---@param z boolean
---@param w boolean
---@return sf.Vector4b
function sf.Vector4b.new(x, y, z, w) end
---@type fun(self: sf.Vector4b): boolean, boolean, boolean, boolean
sf.Vector4b.unpack = function() end
---@type sf.Vector4b
sf.Bvec4 = nil
--- @brief Matrix type, used to set uniforms in GLSL
---@class sf.Mat3
--- Array holding matrix data
---@field array number[]
sf.Mat3 = sf.Mat3 or {}
--- @brief Construct implicitly from SFML transform
---
--- This constructor is only supported for 3x3 and 4x4
--- matrices.
---
--- @param transform Object containing a transform.
---@overload fun(values: number[]): sf.Mat3
---@param transform sf.Transform
---@return sf.Mat3
function sf.Mat3.new(transform) end
---@type fun(source: sf.Transform, dest: sf.Mat3)
sf.Mat3.copyMatrix = function() end
--- @brief Matrix type, used to set uniforms in GLSL
---@class sf.Mat4
--- Array holding matrix data
---@field array number[]
sf.Mat4 = sf.Mat4 or {}
--- @brief Construct implicitly from SFML transform
---
--- This constructor is only supported for 3x3 and 4x4
--- matrices.
---
--- @param transform Object containing a transform.
---@overload fun(values: number[]): sf.Mat4
---@param transform sf.Transform
---@return sf.Mat4
function sf.Mat4.new(transform) end
---@type fun(source: sf.Transform, dest: sf.Mat4)
sf.Mat4.copyMatrix = function() end
--- @brief Define the states used for drawing to a `RenderTarget`
---@class sf.RenderStates
--- Blending mode
---@field blendMode sf.BlendMode
--- Stencil mode
---@field stencilMode sf.StencilMode
--- Transform
---@field transform sf.Transform
--- Texture coordinate type
---@field coordinateType sf.CoordinateType
--- Texture
---@field texture sf.Texture
--- Shader
---@field shader sf.Shader
sf.RenderStates = sf.RenderStates or {}
--- @brief Construct a set of render states with all its attributes
---
--- @param theBlendMode      Blend mode to use
--- @param theStencilMode    Stencil mode to use
--- @param theTransform      Transform to use
--- @param theCoordinateType Texture coordinate type to use
--- @param theTexture        Texture to use
--- @param theShader         Shader to use
---@overload fun(theBlendMode: sf.BlendMode): sf.RenderStates
---@overload fun(theStencilMode: sf.StencilMode): sf.RenderStates
---@overload fun(theTransform: sf.Transform): sf.RenderStates
---@overload fun(theTexture: sf.Texture): sf.RenderStates
---@overload fun(theShader: sf.Shader): sf.RenderStates
---@overload fun(): sf.RenderStates
---@param theBlendMode sf.BlendMode
---@param theStencilMode sf.StencilMode
---@param theTransform sf.Transform
---@param theCoordinateType sf.CoordinateType
---@param theTexture sf.Texture
---@param theShader sf.Shader
---@return sf.RenderStates
function sf.RenderStates.new(theBlendMode, theStencilMode, theTransform, theCoordinateType, theTexture, theShader) end
--- Special instance holding the default render states
---@type sf.RenderStates
sf.RenderStates.Default = nil
--- @brief Decomposed transform defined by a position, a rotation and a scale
---@class sf.Transformable
sf.Transformable = sf.Transformable or {}
--- @brief Default constructor
---@type fun(): sf.Transformable
sf.Transformable.new = function() end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.Transformable, position: sf.Vector2f)
sf.Transformable.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.Transformable, angle: sf.Angle)
sf.Transformable.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.Transformable, factors: sf.Vector2f)
sf.Transformable.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.Transformable, origin: sf.Vector2f)
sf.Transformable.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.Transformable): sf.Vector2f
sf.Transformable.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.Transformable): sf.Angle
sf.Transformable.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.Transformable): sf.Vector2f
sf.Transformable.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.Transformable): sf.Vector2f
sf.Transformable.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.Transformable, offset: sf.Vector2f)
sf.Transformable.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.Transformable, angle: sf.Angle)
sf.Transformable.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.Transformable, factor: sf.Vector2f)
sf.Transformable.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.Transformable): sf.Transform
sf.Transformable.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.Transformable): sf.Transform
sf.Transformable.getInverseTransform = function() end
--- @brief Base class for textured shapes with outline
---@class sf.Shape : sf.Drawable, sf.Transformable
sf.Shape = sf.Shape or {}
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.Shape, position: sf.Vector2f)
sf.Shape.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.Shape, angle: sf.Angle)
sf.Shape.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.Shape, factors: sf.Vector2f)
sf.Shape.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.Shape, origin: sf.Vector2f)
sf.Shape.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.Shape): sf.Vector2f
sf.Shape.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.Shape): sf.Angle
sf.Shape.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.Shape): sf.Vector2f
sf.Shape.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.Shape): sf.Vector2f
sf.Shape.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.Shape, offset: sf.Vector2f)
sf.Shape.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.Shape, angle: sf.Angle)
sf.Shape.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.Shape, factor: sf.Vector2f)
sf.Shape.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.Shape): sf.Transform
sf.Shape.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.Shape): sf.Transform
sf.Shape.getInverseTransform = function() end
--- @brief Change the source texture of the shape
---
--- The `texture` argument refers to a texture that must
--- exist as long as the shape uses it. Indeed, the shape
--- doesn't store its own copy of the texture, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the source texture is destroyed and the shape tries to
--- use it, the behavior is undefined.
--- `texture` can be a null pointer to disable texturing.
--- If `resetRect` is `true`, the `TextureRect` property of
--- the shape is automatically adjusted to the size of the new
--- texture. If it is `false`, the texture rect is left unchanged.
---
--- @param texture   New texture
--- @param resetRect Should the texture rect be reset to the size of the new texture?
---
--- @see `getTexture`, `setTextureRect`
---@overload fun(self: sf.Shape, texture: sf.Texture)
---@param self sf.Shape
---@param texture sf.Texture
---@param resetRect boolean
function sf.Shape.setTexture(self, texture, resetRect) end
--- @brief Set the sub-rectangle of the texture that the shape will display
---
--- The texture rect is useful when you don't want to display
--- the whole texture, but rather a part of it.
--- By default, the texture rect covers the entire texture.
---
--- @param rect Rectangle defining the region of the texture to display
---
--- @see `getTextureRect`, `setTexture`
---@type fun(self: sf.Shape, rect: sf.IntRect)
sf.Shape.setTextureRect = function() end
--- @brief Set the fill color of the shape
---
--- This color is modulated (multiplied) with the shape's
--- texture if any. It can be used to colorize the shape,
--- or change its global opacity.
--- You can use `sf::Color::Transparent` to make the inside of
--- the shape transparent, and have the outline alone.
--- By default, the shape's fill color is opaque white.
---
--- @param color New color of the shape
---
--- @see `getFillColor`, `setOutlineColor`
---@type fun(self: sf.Shape, color: sf.Color)
sf.Shape.setFillColor = function() end
--- @brief Set the outline color of the shape
---
--- By default, the shape's outline color is opaque white.
---
--- @param color New outline color of the shape
---
--- @see `getOutlineColor`, `setFillColor`
---@type fun(self: sf.Shape, color: sf.Color)
sf.Shape.setOutlineColor = function() end
--- @brief Set the thickness of the shape's outline
---
--- Note that negative values are allowed (so that the outline
--- expands towards the center of the shape), and using zero
--- disables the outline.
--- By default, the outline thickness is 0.
---
--- @param thickness New outline thickness
---
--- @see `getOutlineThickness`
---@type fun(self: sf.Shape, thickness: number)
sf.Shape.setOutlineThickness = function() end
--- @brief Set the limit on the ratio between miter length and outline thickness
---
--- Outline segments around each shape corner are joined either
--- with a miter or a bevel join.
--- - A miter join is formed by extending outline segments until
--- they intersect. The distance between the point of
--- intersection and the shape's corner is the miter length.
--- - A bevel join is formed by connecting outline segments with
--- a straight line perpendicular to the corner's bissector.
---
--- The miter limit is used to determine whether ouline segments
--- around a corner are joined with a bevel or a miter.
--- When the ratio between the miter length and outline thickness
--- exceeds the miter limit, a bevel is used instead of a miter.
---
--- The miter limit is linked to the maximum inner angle of a
--- corner below which a bevel is used by the following formula:
---
--- miterLimit = 1 / sin(angle / 2)
---
--- The miter limit must be greater than or equal to 1.
--- By default, the miter limit is 10.
---
--- @param miterLimit New miter limit
---
--- @see getMiterLimit
---@type fun(self: sf.Shape, miterLimit: number)
sf.Shape.setMiterLimit = function() end
--- @brief Get the source texture of the shape
---
--- If the shape has no source texture, a `nullptr` is returned.
--- The returned pointer is const, which means that you can't
--- modify the texture when you retrieve it with this function.
---
--- @return Pointer to the shape's texture
---
--- @see `setTexture`
---@type fun(self: sf.Shape): sf.Texture
sf.Shape.getTexture = function() end
--- @brief Get the sub-rectangle of the texture displayed by the shape
---
--- @return Texture rectangle of the shape
---
--- @see `setTextureRect`
---@type fun(self: sf.Shape): sf.IntRect
sf.Shape.getTextureRect = function() end
--- @brief Get the fill color of the shape
---
--- @return Fill color of the shape
---
--- @see `setFillColor`
---@type fun(self: sf.Shape): sf.Color
sf.Shape.getFillColor = function() end
--- @brief Get the outline color of the shape
---
--- @return Outline color of the shape
---
--- @see `setOutlineColor`
---@type fun(self: sf.Shape): sf.Color
sf.Shape.getOutlineColor = function() end
--- @brief Get the outline thickness of the shape
---
--- @return Outline thickness of the shape
---
--- @see `setOutlineThickness`
---@type fun(self: sf.Shape): number
sf.Shape.getOutlineThickness = function() end
--- @brief Get the limit on the ratio between miter length and outline thickness
---
--- @return Limit on the ratio between miter length and outline thickness
---
--- @see setMiterLimit
---@type fun(self: sf.Shape): number
sf.Shape.getMiterLimit = function() end
--- @brief Get the total number of points of the shape
---
--- @return Number of points of the shape
---
--- @see `getPoint`
---@type fun(self: sf.Shape): integer
sf.Shape.getPointCount = function() end
--- @brief Get a point of the shape
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
--- The result is undefined if `index` is out of the valid range.
---
--- @param index Index of the point to get, in range [0 .. getPointCount() - 1]
---
--- @return `index`-th point of the shape
---
--- @see `getPointCount`
---@type fun(self: sf.Shape, index: integer): sf.Vector2f
sf.Shape.getPoint = function() end
--- @brief Get the geometric center of the shape
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
---
--- @return The geometric center of the shape
---@type fun(self: sf.Shape): sf.Vector2f
sf.Shape.getGeometricCenter = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.Shape): sf.FloatRect
sf.Shape.getLocalBounds = function() end
--- @brief Get the global (non-minimal) bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- shape in the global 2D world's coordinate system.
---
--- This function does not necessarily return the _minimal_
--- bounding rectangle. It merely ensures that the returned
--- rectangle covers all the vertices (but possibly more).
--- This allows for a fast approximation of the bounds as a
--- first check; you may want to use more precise checks
--- on top of that.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.Shape): sf.FloatRect
sf.Shape.getGlobalBounds = function() end
--- @brief Specialized shape representing a circle
---@class sf.CircleShape : sf.Shape, sf.Drawable, sf.Transformable
sf.CircleShape = sf.CircleShape or {}
--- @brief Default constructor
---
--- @param radius     Radius of the circle
--- @param pointCount Number of points composing the circle
---@overload fun(radius: number): sf.CircleShape
---@overload fun(): sf.CircleShape
---@param radius number
---@param pointCount integer
---@return sf.CircleShape
function sf.CircleShape.new(radius, pointCount) end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.CircleShape, position: sf.Vector2f)
sf.CircleShape.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.CircleShape, angle: sf.Angle)
sf.CircleShape.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.CircleShape, factors: sf.Vector2f)
sf.CircleShape.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.CircleShape, origin: sf.Vector2f)
sf.CircleShape.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.CircleShape): sf.Vector2f
sf.CircleShape.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.CircleShape): sf.Angle
sf.CircleShape.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.CircleShape): sf.Vector2f
sf.CircleShape.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.CircleShape): sf.Vector2f
sf.CircleShape.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.CircleShape, offset: sf.Vector2f)
sf.CircleShape.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.CircleShape, angle: sf.Angle)
sf.CircleShape.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.CircleShape, factor: sf.Vector2f)
sf.CircleShape.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.CircleShape): sf.Transform
sf.CircleShape.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.CircleShape): sf.Transform
sf.CircleShape.getInverseTransform = function() end
--- @brief Change the source texture of the shape
---
--- The `texture` argument refers to a texture that must
--- exist as long as the shape uses it. Indeed, the shape
--- doesn't store its own copy of the texture, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the source texture is destroyed and the shape tries to
--- use it, the behavior is undefined.
--- `texture` can be a null pointer to disable texturing.
--- If `resetRect` is `true`, the `TextureRect` property of
--- the shape is automatically adjusted to the size of the new
--- texture. If it is `false`, the texture rect is left unchanged.
---
--- @param texture   New texture
--- @param resetRect Should the texture rect be reset to the size of the new texture?
---
--- @see `getTexture`, `setTextureRect`
---@overload fun(self: sf.CircleShape, texture: sf.Texture)
---@param self sf.CircleShape
---@param texture sf.Texture
---@param resetRect boolean
function sf.CircleShape.setTexture(self, texture, resetRect) end
--- @brief Set the sub-rectangle of the texture that the shape will display
---
--- The texture rect is useful when you don't want to display
--- the whole texture, but rather a part of it.
--- By default, the texture rect covers the entire texture.
---
--- @param rect Rectangle defining the region of the texture to display
---
--- @see `getTextureRect`, `setTexture`
---@type fun(self: sf.CircleShape, rect: sf.IntRect)
sf.CircleShape.setTextureRect = function() end
--- @brief Set the fill color of the shape
---
--- This color is modulated (multiplied) with the shape's
--- texture if any. It can be used to colorize the shape,
--- or change its global opacity.
--- You can use `sf::Color::Transparent` to make the inside of
--- the shape transparent, and have the outline alone.
--- By default, the shape's fill color is opaque white.
---
--- @param color New color of the shape
---
--- @see `getFillColor`, `setOutlineColor`
---@type fun(self: sf.CircleShape, color: sf.Color)
sf.CircleShape.setFillColor = function() end
--- @brief Set the outline color of the shape
---
--- By default, the shape's outline color is opaque white.
---
--- @param color New outline color of the shape
---
--- @see `getOutlineColor`, `setFillColor`
---@type fun(self: sf.CircleShape, color: sf.Color)
sf.CircleShape.setOutlineColor = function() end
--- @brief Set the thickness of the shape's outline
---
--- Note that negative values are allowed (so that the outline
--- expands towards the center of the shape), and using zero
--- disables the outline.
--- By default, the outline thickness is 0.
---
--- @param thickness New outline thickness
---
--- @see `getOutlineThickness`
---@type fun(self: sf.CircleShape, thickness: number)
sf.CircleShape.setOutlineThickness = function() end
--- @brief Set the limit on the ratio between miter length and outline thickness
---
--- Outline segments around each shape corner are joined either
--- with a miter or a bevel join.
--- - A miter join is formed by extending outline segments until
--- they intersect. The distance between the point of
--- intersection and the shape's corner is the miter length.
--- - A bevel join is formed by connecting outline segments with
--- a straight line perpendicular to the corner's bissector.
---
--- The miter limit is used to determine whether ouline segments
--- around a corner are joined with a bevel or a miter.
--- When the ratio between the miter length and outline thickness
--- exceeds the miter limit, a bevel is used instead of a miter.
---
--- The miter limit is linked to the maximum inner angle of a
--- corner below which a bevel is used by the following formula:
---
--- miterLimit = 1 / sin(angle / 2)
---
--- The miter limit must be greater than or equal to 1.
--- By default, the miter limit is 10.
---
--- @param miterLimit New miter limit
---
--- @see getMiterLimit
---@type fun(self: sf.CircleShape, miterLimit: number)
sf.CircleShape.setMiterLimit = function() end
--- @brief Get the source texture of the shape
---
--- If the shape has no source texture, a `nullptr` is returned.
--- The returned pointer is const, which means that you can't
--- modify the texture when you retrieve it with this function.
---
--- @return Pointer to the shape's texture
---
--- @see `setTexture`
---@type fun(self: sf.CircleShape): sf.Texture
sf.CircleShape.getTexture = function() end
--- @brief Get the sub-rectangle of the texture displayed by the shape
---
--- @return Texture rectangle of the shape
---
--- @see `setTextureRect`
---@type fun(self: sf.CircleShape): sf.IntRect
sf.CircleShape.getTextureRect = function() end
--- @brief Get the fill color of the shape
---
--- @return Fill color of the shape
---
--- @see `setFillColor`
---@type fun(self: sf.CircleShape): sf.Color
sf.CircleShape.getFillColor = function() end
--- @brief Get the outline color of the shape
---
--- @return Outline color of the shape
---
--- @see `setOutlineColor`
---@type fun(self: sf.CircleShape): sf.Color
sf.CircleShape.getOutlineColor = function() end
--- @brief Get the outline thickness of the shape
---
--- @return Outline thickness of the shape
---
--- @see `setOutlineThickness`
---@type fun(self: sf.CircleShape): number
sf.CircleShape.getOutlineThickness = function() end
--- @brief Get the limit on the ratio between miter length and outline thickness
---
--- @return Limit on the ratio between miter length and outline thickness
---
--- @see setMiterLimit
---@type fun(self: sf.CircleShape): number
sf.CircleShape.getMiterLimit = function() end
--- @brief Get the number of points of the circle
---
--- @return Number of points of the circle
---
--- @see `setPointCount`
---@type fun(self: sf.CircleShape): integer
sf.CircleShape.getPointCount = function() end
--- @brief Get a point of the circle
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
--- The result is undefined if `index` is out of the valid range.
---
--- @param index Index of the point to get, in range [0 .. getPointCount() - 1]
---
--- @return `index`-th point of the shape
---@type fun(self: sf.CircleShape, index: integer): sf.Vector2f
sf.CircleShape.getPoint = function() end
--- @brief Get the geometric center of the circle
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
---
--- @return The geometric center of the shape
---@type fun(self: sf.CircleShape): sf.Vector2f
sf.CircleShape.getGeometricCenter = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.CircleShape): sf.FloatRect
sf.CircleShape.getLocalBounds = function() end
--- @brief Get the global (non-minimal) bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- shape in the global 2D world's coordinate system.
---
--- This function does not necessarily return the _minimal_
--- bounding rectangle. It merely ensures that the returned
--- rectangle covers all the vertices (but possibly more).
--- This allows for a fast approximation of the bounds as a
--- first check; you may want to use more precise checks
--- on top of that.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.CircleShape): sf.FloatRect
sf.CircleShape.getGlobalBounds = function() end
--- @brief Set the radius of the circle
---
--- @param radius New radius of the circle
---
--- @see `getRadius`
---@type fun(self: sf.CircleShape, radius: number)
sf.CircleShape.setRadius = function() end
--- @brief Get the radius of the circle
---
--- @return Radius of the circle
---
--- @see `setRadius`
---@type fun(self: sf.CircleShape): number
sf.CircleShape.getRadius = function() end
--- @brief Set the number of points of the circle
---
--- @param count New number of points of the circle
---
--- @see `getPointCount`
---@type fun(self: sf.CircleShape, count: integer)
sf.CircleShape.setPointCount = function() end
--- @brief Specialized shape representing a convex polygon
---@class sf.ConvexShape : sf.Shape, sf.Drawable, sf.Transformable
sf.ConvexShape = sf.ConvexShape or {}
--- @brief Default constructor
---
--- @param pointCount Number of points of the polygon
---@overload fun(): sf.ConvexShape
---@param pointCount integer
---@return sf.ConvexShape
function sf.ConvexShape.new(pointCount) end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.ConvexShape, position: sf.Vector2f)
sf.ConvexShape.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.ConvexShape, angle: sf.Angle)
sf.ConvexShape.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.ConvexShape, factors: sf.Vector2f)
sf.ConvexShape.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.ConvexShape, origin: sf.Vector2f)
sf.ConvexShape.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.ConvexShape): sf.Vector2f
sf.ConvexShape.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.ConvexShape): sf.Angle
sf.ConvexShape.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.ConvexShape): sf.Vector2f
sf.ConvexShape.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.ConvexShape): sf.Vector2f
sf.ConvexShape.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.ConvexShape, offset: sf.Vector2f)
sf.ConvexShape.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.ConvexShape, angle: sf.Angle)
sf.ConvexShape.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.ConvexShape, factor: sf.Vector2f)
sf.ConvexShape.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.ConvexShape): sf.Transform
sf.ConvexShape.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.ConvexShape): sf.Transform
sf.ConvexShape.getInverseTransform = function() end
--- @brief Change the source texture of the shape
---
--- The `texture` argument refers to a texture that must
--- exist as long as the shape uses it. Indeed, the shape
--- doesn't store its own copy of the texture, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the source texture is destroyed and the shape tries to
--- use it, the behavior is undefined.
--- `texture` can be a null pointer to disable texturing.
--- If `resetRect` is `true`, the `TextureRect` property of
--- the shape is automatically adjusted to the size of the new
--- texture. If it is `false`, the texture rect is left unchanged.
---
--- @param texture   New texture
--- @param resetRect Should the texture rect be reset to the size of the new texture?
---
--- @see `getTexture`, `setTextureRect`
---@overload fun(self: sf.ConvexShape, texture: sf.Texture)
---@param self sf.ConvexShape
---@param texture sf.Texture
---@param resetRect boolean
function sf.ConvexShape.setTexture(self, texture, resetRect) end
--- @brief Set the sub-rectangle of the texture that the shape will display
---
--- The texture rect is useful when you don't want to display
--- the whole texture, but rather a part of it.
--- By default, the texture rect covers the entire texture.
---
--- @param rect Rectangle defining the region of the texture to display
---
--- @see `getTextureRect`, `setTexture`
---@type fun(self: sf.ConvexShape, rect: sf.IntRect)
sf.ConvexShape.setTextureRect = function() end
--- @brief Set the fill color of the shape
---
--- This color is modulated (multiplied) with the shape's
--- texture if any. It can be used to colorize the shape,
--- or change its global opacity.
--- You can use `sf::Color::Transparent` to make the inside of
--- the shape transparent, and have the outline alone.
--- By default, the shape's fill color is opaque white.
---
--- @param color New color of the shape
---
--- @see `getFillColor`, `setOutlineColor`
---@type fun(self: sf.ConvexShape, color: sf.Color)
sf.ConvexShape.setFillColor = function() end
--- @brief Set the outline color of the shape
---
--- By default, the shape's outline color is opaque white.
---
--- @param color New outline color of the shape
---
--- @see `getOutlineColor`, `setFillColor`
---@type fun(self: sf.ConvexShape, color: sf.Color)
sf.ConvexShape.setOutlineColor = function() end
--- @brief Set the thickness of the shape's outline
---
--- Note that negative values are allowed (so that the outline
--- expands towards the center of the shape), and using zero
--- disables the outline.
--- By default, the outline thickness is 0.
---
--- @param thickness New outline thickness
---
--- @see `getOutlineThickness`
---@type fun(self: sf.ConvexShape, thickness: number)
sf.ConvexShape.setOutlineThickness = function() end
--- @brief Set the limit on the ratio between miter length and outline thickness
---
--- Outline segments around each shape corner are joined either
--- with a miter or a bevel join.
--- - A miter join is formed by extending outline segments until
--- they intersect. The distance between the point of
--- intersection and the shape's corner is the miter length.
--- - A bevel join is formed by connecting outline segments with
--- a straight line perpendicular to the corner's bissector.
---
--- The miter limit is used to determine whether ouline segments
--- around a corner are joined with a bevel or a miter.
--- When the ratio between the miter length and outline thickness
--- exceeds the miter limit, a bevel is used instead of a miter.
---
--- The miter limit is linked to the maximum inner angle of a
--- corner below which a bevel is used by the following formula:
---
--- miterLimit = 1 / sin(angle / 2)
---
--- The miter limit must be greater than or equal to 1.
--- By default, the miter limit is 10.
---
--- @param miterLimit New miter limit
---
--- @see getMiterLimit
---@type fun(self: sf.ConvexShape, miterLimit: number)
sf.ConvexShape.setMiterLimit = function() end
--- @brief Get the source texture of the shape
---
--- If the shape has no source texture, a `nullptr` is returned.
--- The returned pointer is const, which means that you can't
--- modify the texture when you retrieve it with this function.
---
--- @return Pointer to the shape's texture
---
--- @see `setTexture`
---@type fun(self: sf.ConvexShape): sf.Texture
sf.ConvexShape.getTexture = function() end
--- @brief Get the sub-rectangle of the texture displayed by the shape
---
--- @return Texture rectangle of the shape
---
--- @see `setTextureRect`
---@type fun(self: sf.ConvexShape): sf.IntRect
sf.ConvexShape.getTextureRect = function() end
--- @brief Get the fill color of the shape
---
--- @return Fill color of the shape
---
--- @see `setFillColor`
---@type fun(self: sf.ConvexShape): sf.Color
sf.ConvexShape.getFillColor = function() end
--- @brief Get the outline color of the shape
---
--- @return Outline color of the shape
---
--- @see `setOutlineColor`
---@type fun(self: sf.ConvexShape): sf.Color
sf.ConvexShape.getOutlineColor = function() end
--- @brief Get the outline thickness of the shape
---
--- @return Outline thickness of the shape
---
--- @see `setOutlineThickness`
---@type fun(self: sf.ConvexShape): number
sf.ConvexShape.getOutlineThickness = function() end
--- @brief Get the limit on the ratio between miter length and outline thickness
---
--- @return Limit on the ratio between miter length and outline thickness
---
--- @see setMiterLimit
---@type fun(self: sf.ConvexShape): number
sf.ConvexShape.getMiterLimit = function() end
--- @brief Get the number of points of the polygon
---
--- @return Number of points of the polygon
---
--- @see `setPointCount`
---@type fun(self: sf.ConvexShape): integer
sf.ConvexShape.getPointCount = function() end
--- @brief Get the position of a point
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
--- The result is undefined if `index` is out of the valid range.
---
--- @param index Index of the point to get, in range [0 .. getPointCount() - 1]
---
--- @return Position of the `index`-th point of the polygon
---
--- @see `setPoint`
---@type fun(self: sf.ConvexShape, index: integer): sf.Vector2f
sf.ConvexShape.getPoint = function() end
--- @brief Get the geometric center of the shape
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
---
--- @return The geometric center of the shape
---@type fun(self: sf.ConvexShape): sf.Vector2f
sf.ConvexShape.getGeometricCenter = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.ConvexShape): sf.FloatRect
sf.ConvexShape.getLocalBounds = function() end
--- @brief Get the global (non-minimal) bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- shape in the global 2D world's coordinate system.
---
--- This function does not necessarily return the _minimal_
--- bounding rectangle. It merely ensures that the returned
--- rectangle covers all the vertices (but possibly more).
--- This allows for a fast approximation of the bounds as a
--- first check; you may want to use more precise checks
--- on top of that.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.ConvexShape): sf.FloatRect
sf.ConvexShape.getGlobalBounds = function() end
--- @brief Set the number of points of the polygon
---
--- For the shape to be rendered as expected, `count` must
--- be greater or equal to 3.
---
--- @param count New number of points of the polygon
---
--- @see `getPointCount`
---@type fun(self: sf.ConvexShape, count: integer)
sf.ConvexShape.setPointCount = function() end
--- @brief Set the position of a point
---
--- Don't forget that the shape must be convex and the
--- order of points matters. Points should not overlap.
--- This applies to rendering; it is explicitly allowed
--- to temporarily have non-convex or degenerate shapes
--- when not drawn (e.g. during shape initialization).
---
--- Point count must be specified beforehand. The behavior is
--- undefined if `index` is greater than or equal to getPointCount.
---
--- @param index Index of the point to change, in range [0 .. getPointCount() - 1]
--- @param point New position of the point
---
--- @see `getPoint`
---@type fun(self: sf.ConvexShape, index: integer, point: sf.Vector2f)
sf.ConvexShape.setPoint = function() end
--- @brief Specialized shape representing a rectangle
---@class sf.RectangleShape : sf.Shape, sf.Drawable, sf.Transformable
sf.RectangleShape = sf.RectangleShape or {}
--- @brief Default constructor
---
--- @param size Size of the rectangle
---@overload fun(): sf.RectangleShape
---@param size sf.Vector2f
---@return sf.RectangleShape
function sf.RectangleShape.new(size) end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.RectangleShape, position: sf.Vector2f)
sf.RectangleShape.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.RectangleShape, angle: sf.Angle)
sf.RectangleShape.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.RectangleShape, factors: sf.Vector2f)
sf.RectangleShape.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.RectangleShape, origin: sf.Vector2f)
sf.RectangleShape.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.RectangleShape): sf.Vector2f
sf.RectangleShape.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.RectangleShape): sf.Angle
sf.RectangleShape.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.RectangleShape): sf.Vector2f
sf.RectangleShape.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.RectangleShape): sf.Vector2f
sf.RectangleShape.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.RectangleShape, offset: sf.Vector2f)
sf.RectangleShape.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.RectangleShape, angle: sf.Angle)
sf.RectangleShape.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.RectangleShape, factor: sf.Vector2f)
sf.RectangleShape.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.RectangleShape): sf.Transform
sf.RectangleShape.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.RectangleShape): sf.Transform
sf.RectangleShape.getInverseTransform = function() end
--- @brief Change the source texture of the shape
---
--- The `texture` argument refers to a texture that must
--- exist as long as the shape uses it. Indeed, the shape
--- doesn't store its own copy of the texture, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the source texture is destroyed and the shape tries to
--- use it, the behavior is undefined.
--- `texture` can be a null pointer to disable texturing.
--- If `resetRect` is `true`, the `TextureRect` property of
--- the shape is automatically adjusted to the size of the new
--- texture. If it is `false`, the texture rect is left unchanged.
---
--- @param texture   New texture
--- @param resetRect Should the texture rect be reset to the size of the new texture?
---
--- @see `getTexture`, `setTextureRect`
---@overload fun(self: sf.RectangleShape, texture: sf.Texture)
---@param self sf.RectangleShape
---@param texture sf.Texture
---@param resetRect boolean
function sf.RectangleShape.setTexture(self, texture, resetRect) end
--- @brief Set the sub-rectangle of the texture that the shape will display
---
--- The texture rect is useful when you don't want to display
--- the whole texture, but rather a part of it.
--- By default, the texture rect covers the entire texture.
---
--- @param rect Rectangle defining the region of the texture to display
---
--- @see `getTextureRect`, `setTexture`
---@type fun(self: sf.RectangleShape, rect: sf.IntRect)
sf.RectangleShape.setTextureRect = function() end
--- @brief Set the fill color of the shape
---
--- This color is modulated (multiplied) with the shape's
--- texture if any. It can be used to colorize the shape,
--- or change its global opacity.
--- You can use `sf::Color::Transparent` to make the inside of
--- the shape transparent, and have the outline alone.
--- By default, the shape's fill color is opaque white.
---
--- @param color New color of the shape
---
--- @see `getFillColor`, `setOutlineColor`
---@type fun(self: sf.RectangleShape, color: sf.Color)
sf.RectangleShape.setFillColor = function() end
--- @brief Set the outline color of the shape
---
--- By default, the shape's outline color is opaque white.
---
--- @param color New outline color of the shape
---
--- @see `getOutlineColor`, `setFillColor`
---@type fun(self: sf.RectangleShape, color: sf.Color)
sf.RectangleShape.setOutlineColor = function() end
--- @brief Set the thickness of the shape's outline
---
--- Note that negative values are allowed (so that the outline
--- expands towards the center of the shape), and using zero
--- disables the outline.
--- By default, the outline thickness is 0.
---
--- @param thickness New outline thickness
---
--- @see `getOutlineThickness`
---@type fun(self: sf.RectangleShape, thickness: number)
sf.RectangleShape.setOutlineThickness = function() end
--- @brief Set the limit on the ratio between miter length and outline thickness
---
--- Outline segments around each shape corner are joined either
--- with a miter or a bevel join.
--- - A miter join is formed by extending outline segments until
--- they intersect. The distance between the point of
--- intersection and the shape's corner is the miter length.
--- - A bevel join is formed by connecting outline segments with
--- a straight line perpendicular to the corner's bissector.
---
--- The miter limit is used to determine whether ouline segments
--- around a corner are joined with a bevel or a miter.
--- When the ratio between the miter length and outline thickness
--- exceeds the miter limit, a bevel is used instead of a miter.
---
--- The miter limit is linked to the maximum inner angle of a
--- corner below which a bevel is used by the following formula:
---
--- miterLimit = 1 / sin(angle / 2)
---
--- The miter limit must be greater than or equal to 1.
--- By default, the miter limit is 10.
---
--- @param miterLimit New miter limit
---
--- @see getMiterLimit
---@type fun(self: sf.RectangleShape, miterLimit: number)
sf.RectangleShape.setMiterLimit = function() end
--- @brief Get the source texture of the shape
---
--- If the shape has no source texture, a `nullptr` is returned.
--- The returned pointer is const, which means that you can't
--- modify the texture when you retrieve it with this function.
---
--- @return Pointer to the shape's texture
---
--- @see `setTexture`
---@type fun(self: sf.RectangleShape): sf.Texture
sf.RectangleShape.getTexture = function() end
--- @brief Get the sub-rectangle of the texture displayed by the shape
---
--- @return Texture rectangle of the shape
---
--- @see `setTextureRect`
---@type fun(self: sf.RectangleShape): sf.IntRect
sf.RectangleShape.getTextureRect = function() end
--- @brief Get the fill color of the shape
---
--- @return Fill color of the shape
---
--- @see `setFillColor`
---@type fun(self: sf.RectangleShape): sf.Color
sf.RectangleShape.getFillColor = function() end
--- @brief Get the outline color of the shape
---
--- @return Outline color of the shape
---
--- @see `setOutlineColor`
---@type fun(self: sf.RectangleShape): sf.Color
sf.RectangleShape.getOutlineColor = function() end
--- @brief Get the outline thickness of the shape
---
--- @return Outline thickness of the shape
---
--- @see `setOutlineThickness`
---@type fun(self: sf.RectangleShape): number
sf.RectangleShape.getOutlineThickness = function() end
--- @brief Get the limit on the ratio between miter length and outline thickness
---
--- @return Limit on the ratio between miter length and outline thickness
---
--- @see setMiterLimit
---@type fun(self: sf.RectangleShape): number
sf.RectangleShape.getMiterLimit = function() end
--- @brief Get the number of points defining the shape
---
--- @return Number of points of the shape. For rectangle
--- shapes, this number is always 4.
---@type fun(self: sf.RectangleShape): integer
sf.RectangleShape.getPointCount = function() end
--- @brief Get a point of the rectangle
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
--- The result is undefined if `index` is out of the valid range.
---
--- @param index Index of the point to get, in range [0 .. 3]
---
--- @return `index`-th point of the shape
---@type fun(self: sf.RectangleShape, index: integer): sf.Vector2f
sf.RectangleShape.getPoint = function() end
--- @brief Get the geometric center of the rectangle
---
--- The returned point is in local coordinates, that is,
--- the shape's transforms (position, rotation, scale) are
--- not taken into account.
---
--- @return The geometric center of the shape
---@type fun(self: sf.RectangleShape): sf.Vector2f
sf.RectangleShape.getGeometricCenter = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.RectangleShape): sf.FloatRect
sf.RectangleShape.getLocalBounds = function() end
--- @brief Get the global (non-minimal) bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- shape in the global 2D world's coordinate system.
---
--- This function does not necessarily return the _minimal_
--- bounding rectangle. It merely ensures that the returned
--- rectangle covers all the vertices (but possibly more).
--- This allows for a fast approximation of the bounds as a
--- first check; you may want to use more precise checks
--- on top of that.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.RectangleShape): sf.FloatRect
sf.RectangleShape.getGlobalBounds = function() end
--- @brief Set the size of the rectangle
---
--- @param size New size of the rectangle
---
--- @see `getSize`
---@type fun(self: sf.RectangleShape, size: sf.Vector2f)
sf.RectangleShape.setSize = function() end
--- @brief Get the size of the rectangle
---
--- @return Size of the rectangle
---
--- @see `setSize`
---@type fun(self: sf.RectangleShape): sf.Vector2f
sf.RectangleShape.getSize = function() end
--- @brief Drawable representation of a texture, with its
--- own transformations, color, etc.
---@class sf.Sprite : sf.Drawable, sf.Transformable
sf.Sprite = sf.Sprite or {}
--- @brief Construct the sprite from a sub-rectangle of a source texture
---
--- @param texture   Source texture
--- @param rectangle Sub-rectangle of the texture to assign to the sprite
---
--- @see `setTexture`, `setTextureRect`
---@overload fun(texture: sf.Texture): sf.Sprite
---@param texture sf.Texture
---@param rectangle sf.IntRect
---@return sf.Sprite
function sf.Sprite.new(texture, rectangle) end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.Sprite, position: sf.Vector2f)
sf.Sprite.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.Sprite, angle: sf.Angle)
sf.Sprite.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.Sprite, factors: sf.Vector2f)
sf.Sprite.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.Sprite, origin: sf.Vector2f)
sf.Sprite.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.Sprite): sf.Vector2f
sf.Sprite.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.Sprite): sf.Angle
sf.Sprite.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.Sprite): sf.Vector2f
sf.Sprite.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.Sprite): sf.Vector2f
sf.Sprite.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.Sprite, offset: sf.Vector2f)
sf.Sprite.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.Sprite, angle: sf.Angle)
sf.Sprite.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.Sprite, factor: sf.Vector2f)
sf.Sprite.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.Sprite): sf.Transform
sf.Sprite.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.Sprite): sf.Transform
sf.Sprite.getInverseTransform = function() end
--- @brief Change the source texture of the sprite
---
--- The `texture` argument refers to a texture that must
--- exist as long as the sprite uses it. Indeed, the sprite
--- doesn't store its own copy of the texture, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the source texture is destroyed and the sprite tries to
--- use it, the behavior is undefined.
--- If `resetRect` is `true`, the `TextureRect` property of
--- the sprite is automatically adjusted to the size of the new
--- texture. If it is `false`, the texture rect is left unchanged.
---
--- @param texture   New texture
--- @param resetRect Should the texture rect be reset to the size of the new texture?
---
--- @see `getTexture`, `setTextureRect`
---@overload fun(self: sf.Sprite, texture: sf.Texture)
---@param self sf.Sprite
---@param texture sf.Texture
---@param resetRect boolean
function sf.Sprite.setTexture(self, texture, resetRect) end
--- @brief Set the sub-rectangle of the texture that the sprite will display
---
--- The texture rect is useful when you don't want to display
--- the whole texture, but rather a part of it.
--- By default, the texture rect covers the entire texture.
---
--- @param rectangle Rectangle defining the region of the texture to display
---
--- @see `getTextureRect`, `setTexture`
---@type fun(self: sf.Sprite, rectangle: sf.IntRect)
sf.Sprite.setTextureRect = function() end
--- @brief Set the global color of the sprite
---
--- This color is modulated (multiplied) with the sprite's
--- texture. It can be used to colorize the sprite, or change
--- its global opacity.
--- By default, the sprite's color is opaque white.
---
--- @param color New color of the sprite
---
--- @see `getColor`
---@type fun(self: sf.Sprite, color: sf.Color)
sf.Sprite.setColor = function() end
--- @brief Get the source texture of the sprite
---
--- The returned reference is const, which means that you can't
--- modify the texture when you retrieve it with this function.
---
--- @return Reference to the sprite's texture
---
--- @see `setTexture`
---@type fun(self: sf.Sprite): sf.Texture
sf.Sprite.getTexture = function() end
--- @brief Get the sub-rectangle of the texture displayed by the sprite
---
--- @return Texture rectangle of the sprite
---
--- @see `setTextureRect`
---@type fun(self: sf.Sprite): sf.IntRect
sf.Sprite.getTextureRect = function() end
--- @brief Get the global color of the sprite
---
--- @return Global color of the sprite
---
--- @see `setColor`
---@type fun(self: sf.Sprite): sf.Color
sf.Sprite.getColor = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.Sprite): sf.FloatRect
sf.Sprite.getLocalBounds = function() end
--- @brief Get the global bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- sprite in the global 2D world's coordinate system.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.Sprite): sf.FloatRect
sf.Sprite.getGlobalBounds = function() end
--- @brief Point with color and texture coordinates
---
--- By default, the vertex color is white and texture coordinates are (0, 0).
---@class sf.Vertex
--- 2D position of the vertex
---@field position sf.Vector2f
--- Color of the vertex
---@field color sf.Color
--- Coordinates of the texture's pixel to map to the vertex NOLINT(readability-redundant-member-init)
---@field texCoords sf.Vector2f
sf.Vertex = sf.Vertex or {}
---@type fun(): sf.Vertex
sf.Vertex.new = function() end
--- @brief Set of one or more 2D primitives
---@class sf.VertexArray : sf.Drawable
sf.VertexArray = sf.VertexArray or {}
--- @brief Construct the vertex array with a type and an initial number of vertices
---
--- @param type        Type of primitives
--- @param vertexCount Initial number of vertices in the array
---@overload fun(type: sf.PrimitiveType): sf.VertexArray
---@overload fun(): sf.VertexArray
---@param type sf.PrimitiveType
---@param vertexCount integer
---@return sf.VertexArray
function sf.VertexArray.new(type, vertexCount) end
--- @brief Return the vertex count
---
--- @return Number of vertices in the array
---@type fun(self: sf.VertexArray): integer
sf.VertexArray.getVertexCount = function() end
--- @brief Clear the vertex array
---
--- This function removes all the vertices from the array.
--- It doesn't deallocate the corresponding memory, so that
--- adding new vertices after clearing doesn't involve
--- reallocating all the memory.
---@type fun(self: sf.VertexArray)
sf.VertexArray.clear = function() end
--- @brief Resize the vertex array
---
--- If `vertexCount` is greater than the current size, the previous
--- vertices are kept and new (default-constructed) vertices are
--- added.
--- If `vertexCount` is less than the current size, existing vertices
--- are removed from the array.
---
--- @param vertexCount New size of the array (number of vertices)
---@type fun(self: sf.VertexArray, vertexCount: integer)
sf.VertexArray.resize = function() end
--- @brief Add a vertex to the array
---
--- @param vertex Vertex to add
---@type fun(self: sf.VertexArray, vertex: sf.Vertex)
sf.VertexArray.append = function() end
--- @brief Set the type of primitives to draw
---
--- This function defines how the vertices must be interpreted
--- when it's time to draw them:
--- @li As points
--- @li As lines
--- @li As triangles
--- The default primitive type is `sf::PrimitiveType::Points`.
---
--- @param type Type of primitive
---@type fun(self: sf.VertexArray, type: sf.PrimitiveType)
sf.VertexArray.setPrimitiveType = function() end
--- @brief Get the type of primitives drawn by the vertex array
---
--- @return Primitive type
---@type fun(self: sf.VertexArray): sf.PrimitiveType
sf.VertexArray.getPrimitiveType = function() end
--- @brief Compute the bounding rectangle of the vertex array
---
--- This function returns the minimal axis-aligned rectangle
--- that contains all the vertices of the array.
---
--- @return Bounding rectangle of the vertex array
---@type fun(self: sf.VertexArray): sf.FloatRect
sf.VertexArray.getBounds = function() end

---@class sf.VertexArray
---@operator get(integer): sf.Vertex

---@class sf.VertexArray
---@field [integer] sf.Vertex

---@class sf.VertexArray
---@operator set(integer, sf.Vertex)
--- @brief Graphical text that can be drawn to a render target
---@class sf.Text : sf.Drawable, sf.Transformable
sf.Text = sf.Text or {}
--- @brief Construct the text from a string, font and size
---
--- Note that if the used font is a bitmap font, it is not
--- scalable, thus not all requested sizes will be available
--- to use. This needs to be taken into consideration when
--- setting the character size. If you need to display text
--- of a certain size, make sure the corresponding bitmap
--- font that supports that size is used.
---
--- @param string         Text assigned to the string
--- @param font           Font used to draw the string
--- @param characterSize  Base size of characters, in pixels
---@overload fun(font: sf.Font, string: string): sf.Text
---@overload fun(font: sf.Font): sf.Text
---@param font sf.Font
---@param string string
---@param characterSize integer
---@return sf.Text
function sf.Text.new(font, string, characterSize) end
--- @brief set the position of the object
---
--- This function completely overwrites the previous position.
--- See the move function to apply an offset based on the previous position instead.
--- The default position of a transformable object is (0, 0).
---
--- Note that `sf::Text` may appear offset when positioned.
--- This is because its local bounds are influenced by font metrics (e.g. tallest characters)
--- to consistently align with the text's baseline. As such the `getGlobalBounds()`
--- position may not match the position you set.
---
--- To account for this offset, the local bounds need to be considered.
--- Either by including it in the position calculation:
--- @code
--- text.setPosition(position - text.getLocalBounds().position);
--- @endcode
--- Or by adjusting the text's origin:
--- @code
--- text.setOrigin(text.getLocalBounds().position);
--- text.setPosition(position);
--- @endcode
---
--- @param position New position
---
--- @see `move`, `getPosition`
---@type fun(self: sf.Text, position: sf.Vector2f)
sf.Text.setPosition = function() end
--- @brief set the orientation of the object
---
--- This function completely overwrites the previous rotation.
--- See the rotate function to add an angle based on the previous rotation instead.
--- The default rotation of a transformable object is 0.
---
--- @param angle New rotation
---
--- @see `rotate`, `getRotation`
---@type fun(self: sf.Text, angle: sf.Angle)
sf.Text.setRotation = function() end
--- @brief set the scale factors of the object
---
--- This function completely overwrites the previous scale.
--- See the scale function to add a factor based on the previous scale instead.
--- The default scale of a transformable object is (1, 1).
---
--- @param factors New scale factors
---
--- @see `scale`, `getScale`
---@type fun(self: sf.Text, factors: sf.Vector2f)
sf.Text.setScale = function() end
--- @brief set the local origin of the object
---
--- The origin of an object defines the center point for
--- all transformations (position, scale, rotation).
--- The coordinates of this point must be relative to the
--- top-left corner of the object, and ignore all
--- transformations (position, scale, rotation).
--- The default origin of a transformable object is (0, 0).
---
--- @param origin New origin
---
--- @see `getOrigin`
---@type fun(self: sf.Text, origin: sf.Vector2f)
sf.Text.setOrigin = function() end
--- @brief get the position of the object
---
--- @return Current position
---
--- @see `setPosition`
---@type fun(self: sf.Text): sf.Vector2f
sf.Text.getPosition = function() end
--- @brief get the orientation of the object
---
--- The rotation is always in the range [0, 360].
---
--- @return Current rotation
---
--- @see `setRotation`
---@type fun(self: sf.Text): sf.Angle
sf.Text.getRotation = function() end
--- @brief get the current scale of the object
---
--- @return Current scale factors
---
--- @see `setScale`
---@type fun(self: sf.Text): sf.Vector2f
sf.Text.getScale = function() end
--- @brief get the local origin of the object
---
--- @return Current origin
---
--- @see `setOrigin`
---@type fun(self: sf.Text): sf.Vector2f
sf.Text.getOrigin = function() end
--- @brief Move the object by a given offset
---
--- This function adds to the current position of the object,
--- unlike `setPosition` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setPosition(object.getPosition() + offset);
--- @endcode
---
--- @param offset Offset
---
--- @see `setPosition`
---@type fun(self: sf.Text, offset: sf.Vector2f)
sf.Text.move = function() end
--- @brief Rotate the object
---
--- This function adds to the current rotation of the object,
--- unlike `setRotation` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- object.setRotation(object.getRotation() + angle);
--- @endcode
---
--- @param angle Angle of rotation
---@type fun(self: sf.Text, angle: sf.Angle)
sf.Text.rotate = function() end
--- @brief Scale the object
---
--- This function multiplies the current scale of the object,
--- unlike `setScale` which overwrites it.
--- Thus, it is equivalent to the following code:
--- @code
--- sf::Vector2f scale = object.getScale();
--- object.setScale(scale.x * factor.x, scale.y * factor.y);
--- @endcode
---
--- @param factor Scale factors
---
--- @see `setScale`
---@type fun(self: sf.Text, factor: sf.Vector2f)
sf.Text.scale = function() end
--- @brief get the combined transform of the object
---
--- @return Transform combining the position/rotation/scale/origin of the object
---
--- @see `getInverseTransform`
---@type fun(self: sf.Text): sf.Transform
sf.Text.getTransform = function() end
--- @brief get the inverse of the combined transform of the object
---
--- @return Inverse of the combined transformations applied to the object
---
--- @see `getTransform`
---@type fun(self: sf.Text): sf.Transform
sf.Text.getInverseTransform = function() end
--- @brief Set the text's string
---
--- The `string` argument is a `sf::String`, which can
--- automatically be constructed from standard string types.
--- So, the following calls are all valid:
--- @code
--- text.setString("hello");
--- text.setString(L"hello");
--- text.setString(std::string("hello"));
--- text.setString(std::wstring(L"hello"));
--- @endcode
--- A text's string is empty by default.
---
--- @param string New string
---
--- @see `getString`
---@type fun(self: sf.Text, string: string)
sf.Text.setString = function() end
--- @brief Set the text's font
---
--- The `font` argument refers to a font that must
--- exist as long as the text uses it. Indeed, the text
--- doesn't store its own copy of the font, but rather keeps
--- a pointer to the one that you passed to this function.
--- If the font is destroyed and the text tries to
--- use it, the behavior is undefined.
---
--- @param font New font
---
--- @see `getFont`
---@type fun(self: sf.Text, font: sf.Font)
sf.Text.setFont = function() end
--- @brief Set the character size
---
--- The default size is 30.
---
--- Note that if the used font is a bitmap font, it is not
--- scalable, thus not all requested sizes will be available
--- to use. This needs to be taken into consideration when
--- setting the character size. If you need to display text
--- of a certain size, make sure the corresponding bitmap
--- font that supports that size is used.
---
--- @param size New character size, in pixels
---
--- @see `getCharacterSize`
---@type fun(self: sf.Text, size: integer)
sf.Text.setCharacterSize = function() end
--- @brief Set the line spacing factor
---
--- The default spacing between lines is defined by the font.
--- This method enables you to set a factor for the spacing
--- between lines. By default the line spacing factor is 1.
---
--- @param spacingFactor New line spacing factor
---
--- @see `getLineSpacing`
---@type fun(self: sf.Text, spacingFactor: number)
sf.Text.setLineSpacing = function() end
--- @brief Set the letter spacing factor
---
--- The default spacing between letters is defined by the font.
--- This factor doesn't directly apply to the existing
--- spacing between each character, it rather adds a fixed
--- space between them which is calculated from the font
--- metrics and the character size.
--- Note that factors below 1 (including negative numbers) bring
--- characters closer to each other.
--- By default the letter spacing factor is 1.
---
--- @param spacingFactor New letter spacing factor
---
--- @see `getLetterSpacing`
---@type fun(self: sf.Text, spacingFactor: number)
sf.Text.setLetterSpacing = function() end
--- @brief Set the text's style
---
--- You can pass a combination of one or more styles, for
--- example `sf::Text::Bold | sf::Text::Italic`.
--- The default style is `sf::Text::Regular`.
---
--- @param style New style
---
--- @see `getStyle`
---@type fun(self: sf.Text, style: integer)
sf.Text.setStyle = function() end
--- @brief Set the fill color of the text
---
--- By default, the text's fill color is opaque white.
--- Setting the fill color to a transparent color with an outline
--- will cause the outline to be displayed in the fill area of the text.
---
--- @param color New fill color of the text
---
--- @see `getFillColor`
---@type fun(self: sf.Text, color: sf.Color)
sf.Text.setFillColor = function() end
--- @brief Set the outline color of the text
---
--- By default, the text's outline color is opaque black.
---
--- @param color New outline color of the text
---
--- @see `getOutlineColor`
---@type fun(self: sf.Text, color: sf.Color)
sf.Text.setOutlineColor = function() end
--- @brief Set the thickness of the text's outline
---
--- By default, the outline thickness is 0.
---
--- Be aware that using a negative value for the outline
--- thickness will cause distorted rendering.
---
--- @param thickness New outline thickness, in pixels
---
--- @see `getOutlineThickness`
---@type fun(self: sf.Text, thickness: number)
sf.Text.setOutlineThickness = function() end
--- @brief Set the line alignment for a multi-line text
---
--- By default, the lines will be aligned according to the
--- direction of the line's script. Left-to-right scripts
--- will be aligned to the left and right-to-left scripts
--- will be aligned to the right.
---
--- Forcing alignment will ignore script direction and always
--- align according to the requested line alignment.
---
--- @param lineAlignment New line alignment
---
--- @see `getLineAlignment`
---@type fun(self: sf.Text, lineAlignment: sf.Text.LineAlignment)
sf.Text.setLineAlignment = function() end
--- @brief Set the text orientation
---
--- By default, the lines will have horizontal orientation.
---
--- Be aware that most fonts don't natively support vertical
--- orientations. Fonts that are the most likely to natively
--- support vertical orientations are those whose scripts
--- also support vertical orientations e.g. east asian scripts.
---
--- If a font does not natively support vertical orientation,
--- vertical metrics might still be provided for shaping.
--- In this case, they are very likely to be emulated and might
--- not result in good visual output.
---
--- Some metrics such as advance and baseline position will
--- be rotated so they match the vertical axis.
---
--- @param textOrientation New text orientation
---
--- @see `getTextOrientation`
---@type fun(self: sf.Text, textOrientation: sf.Text.TextOrientation)
sf.Text.setTextOrientation = function() end
--- @brief Get the text's string
---
--- The returned string is a `sf::String`, which can automatically
--- be converted to standard string types. So, the following
--- lines of code are all valid:
--- @code
--- sf::String   s1 = text.getString();
--- std::string  s2 = text.getString();
--- std::wstring s3 = text.getString();
--- @endcode
---
--- @return Text's string
---
--- @see `setString`
---@type fun(self: sf.Text): string
sf.Text.getString = function() end
--- @brief Get the text's font
---
--- The returned reference is const, which means that you
--- cannot modify the font when you get it from this function.
---
--- @return Reference to the text's font
---
--- @see `setFont`
---@type fun(self: sf.Text): sf.Font
sf.Text.getFont = function() end
--- @brief Get the character size
---
--- @return Size of the characters, in pixels
---
--- @see `setCharacterSize`
---@type fun(self: sf.Text): integer
sf.Text.getCharacterSize = function() end
--- @brief Get the size of the letter spacing factor
---
--- @return Size of the letter spacing factor
---
--- @see `setLetterSpacing`
---@type fun(self: sf.Text): number
sf.Text.getLetterSpacing = function() end
--- @brief Get the size of the line spacing factor
---
--- @return Size of the line spacing factor
---
--- @see `setLineSpacing`
---@type fun(self: sf.Text): number
sf.Text.getLineSpacing = function() end
--- @brief Get the text's style
---
--- @return Text's style
---
--- @see `setStyle`
---@type fun(self: sf.Text): integer
sf.Text.getStyle = function() end
--- @brief Get the fill color of the text
---
--- @return Fill color of the text
---
--- @see `setFillColor`
---@type fun(self: sf.Text): sf.Color
sf.Text.getFillColor = function() end
--- @brief Get the outline color of the text
---
--- @return Outline color of the text
---
--- @see `setOutlineColor`
---@type fun(self: sf.Text): sf.Color
sf.Text.getOutlineColor = function() end
--- @brief Get the outline thickness of the text
---
--- @return Outline thickness of the text, in pixels
---
--- @see `setOutlineThickness`
---@type fun(self: sf.Text): number
sf.Text.getOutlineThickness = function() end
--- @brief Get the line alignment for a multi-line text
---
--- @return Line alignment
---
--- @see `setLineAlignment`
---@type fun(self: sf.Text): sf.Text.LineAlignment
sf.Text.getLineAlignment = function() end
--- @brief Get the text orientation
---
--- @return Text orientation
---
--- @see `setTextOrientation`
---@type fun(self: sf.Text): sf.Text.TextOrientation
sf.Text.getTextOrientation = function() end
--- @brief Return the position of the `index`-th character
---
--- @deprecated Use `getShapedGlyphs()` instead.
---
--- This function computes the visual position of a character
--- from its index in the string. The returned position is
--- in global coordinates (translation, rotation, scale and
--- origin are applied).
--- If `index` is out of range, the position of the end of
--- the string is returned.
---
--- @param index Index of the character
---
--- @return Position of the character
---@type fun(self: sf.Text, index: integer): sf.Vector2f
sf.Text.findCharacterPos = function() end
--- @brief Return a list of shaped glyphs that make up the text
---
--- The result of shaping i.e. positioning individual glyphs
--- based on the properties of the font and the input text
--- is a sequence of shaped glyphs that each have a collection
--- of properties.
---
--- In addition to the glyph information that is available
--- by looking up a glyph from a font, the glyph position,
--- glyph cluster ID and direction of the text represented
--- by the glyph is provided.
---
--- When specifying unicode text, multiple unicode codepoints
--- might combine to form e.g. a ligature such as æ or
--- base-and-mark sequence such as é which are composed of
--- multiple individual glyphs. These combinations are known
--- as grapheme clusters. When segmenting text into grapheme
--- clusters, each cluster identifies a complete unit of text
--- that will be drawn. There are other methods of segmenting
--- text into clusters e.g. without combining marks.
--- Character cluster segmentation is used as the default.
--- A single grapheme can be represented by an individual
--- codepoint or by a composition of codepoints e.g. an e as
--- the base and an accent as the mark which together compose
--- the grapheme é. The cluster groups that result from shaping
--- depend on whether the input text provides composed
--- codepoints or decomposed codepoints. This is an advanced
--- topic known as unicode normalisation.
---
--- When positioning e.g. a cursor within the text, grapheme
--- clusters can be treated as the basic units of which the
--- text is composed and not subdivided into their individual
--- components or glyphs. If positioning of the cursor within
--- a single grapheme e.g. a ligature is required, a more
--- fine-grained cluster segmentation algorithm should be used.
---
--- The returned glyph positions are in local coordinates
--- (translation, rotation, scale and origin are not applied).
---
--- @return List of shaped glyphs that make up the text
---
--- @see `setClusterGrouping`
---@type fun(self: sf.Text): sf.Text.ShapedGlyph[]
sf.Text.getShapedGlyphs = function() end
--- @brief Return the cluster grouping algorithm in use
---
--- @return The cluster grouping algorithm in use
---@type fun(self: sf.Text): sf.Text.ClusterGrouping
sf.Text.getClusterGrouping = function() end
--- @brief Set the cluster grouping algorithm to use
---
--- By default, character cluster grouping is used.
---
--- Character cluster grouping is good enough to be able to
--- position cursors in most scenarios. If more coarse-grained
--- grouping is required, grapheme grouping can be selected.
---
--- Cluster grouping can also be disabled if necessary.
---
--- @param clusterGrouping The cluster grouping algorithm to use
---@type fun(self: sf.Text, clusterGrouping: sf.Text.ClusterGrouping)
sf.Text.setClusterGrouping = function() end
--- @brief Set the glyph pre-processor to be called per glyph
---
--- The glyph pre-processor is a callable that will be called
--- with glyph data to be pre-processed.
---
--- @param glyphPreProcessor The glyph pre-processor to be called per glyph, pass an empty pre-processor to disable pre-processing
---@type fun(self: sf.Text, glyphPreProcessor: sf.Text.GlyphPreProcessor|nil)
sf.Text.setGlyphPreProcessor = function() end
--- @brief Get a reference to the vertex data of this text
---
--- The vertex data is regenerated by the text whenever it is
--- necessary. Any changes made to the vertex data will be
--- discarded whenever this happens.
---
--- @return Reference to the vertex data of this text
---@type fun(self: sf.Text): sf.VertexArray
sf.Text.getVertexData = function() end
--- @brief Get a reference to the outline vertex data of this text
---
--- The outline vertex data is regenerated by the text whenever
--- it is necessary. Any changes made to the outline vertex data
--- will be discarded whenever this happens.
---
--- @return Reference to the vertex data of this text
---@type fun(self: sf.Text): sf.VertexArray
sf.Text.getOutlineVertexData = function() end
--- @brief Get the local bounding rectangle of the entity
---
--- The returned rectangle is in local coordinates, which means
--- that it ignores the transformations (translation, rotation,
--- scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- entity in the entity's coordinate system.
---
--- @return Local bounding rectangle of the entity
---@type fun(self: sf.Text): sf.FloatRect
sf.Text.getLocalBounds = function() end
--- @brief Get the global bounding rectangle of the entity
---
--- The returned rectangle is in global coordinates, which means
--- that it takes into account the transformations (translation,
--- rotation, scale, ...) that are applied to the entity.
--- In other words, this function returns the bounds of the
--- text in the global 2D world's coordinate system.
---
--- @return Global bounding rectangle of the entity
---@type fun(self: sf.Text): sf.FloatRect
sf.Text.getGlobalBounds = function() end
--- @brief Enumeration of the string drawing styles
---@class sf.Text.Style
--- Regular characters, no style
---@field Regular integer
--- Bold characters
---@field Bold integer
--- Italic characters
---@field Italic integer
--- Underlined characters
---@field Underlined integer
--- Strike through characters
---@field StrikeThrough integer
sf.Text.Style = sf.Text.Style or {}
--- @brief Enumeration of the text alignment options
---@class sf.Text.LineAlignment
--- Automatically align lines by script direction, left-align left-to-right text and right-align right-to-left text
---@field Default sf.Text.LineAlignment
--- Force align all lines to the left, regardless of script direction
---@field Left sf.Text.LineAlignment
--- Force align all lines centrally
---@field Center sf.Text.LineAlignment
--- Force align lines to the right, regardless of script direction
---@field Right sf.Text.LineAlignment
sf.Text.LineAlignment = sf.Text.LineAlignment or {}
--- @brief Cluster Grouping
---@class sf.Text.ClusterGrouping
--- Group clusters by grapheme
---@field Grapheme sf.Text.ClusterGrouping
--- Group clusters by character
---@field Character sf.Text.ClusterGrouping
--- Do not group clusters
---@field None sf.Text.ClusterGrouping
sf.Text.ClusterGrouping = sf.Text.ClusterGrouping or {}
--- @brief Text Direction
---@class sf.Text.TextDirection
--- Unspecified
---@field Unspecified sf.Text.TextDirection
--- Left-to-right
---@field LeftToRight sf.Text.TextDirection
--- Right-to-left
---@field RightToLeft sf.Text.TextDirection
--- Top-to-bottom
---@field TopToBottom sf.Text.TextDirection
--- Bottom-to-top
---@field BottomToTop sf.Text.TextDirection
sf.Text.TextDirection = sf.Text.TextDirection or {}
--- @brief Text Orientation
---@class sf.Text.TextOrientation
--- Default (left-to-right or right-to-left depending on detected script)
---@field Default sf.Text.TextOrientation
--- Top-to-bottom
---@field TopToBottom sf.Text.TextOrientation
--- Bottom-to-top
---@field BottomToTop sf.Text.TextOrientation
sf.Text.TextOrientation = sf.Text.TextOrientation or {}
--- @brief Structure describing a glyph after shaping
---@class sf.Text.ShapedGlyph
---@field glyph sf.Glyph
--- Position of the glyph within a text
---@field position sf.Vector2f
--- Cluster ID
---@field cluster integer
--- Text direction
---@field textDirection sf.Text.TextDirection
--- The baseline position of the line this glyph is a part of
---@field baseline number
--- Starting offset of the vertex data belonging to this glyph
---@field vertexOffset integer
--- Count of vertices belonging to this glyph
---@field vertexCount integer
sf.Text.ShapedGlyph = sf.Text.ShapedGlyph or {}
---@type fun(): sf.Text.ShapedGlyph
sf.Text.ShapedGlyph.new = function() end
--- @brief Callable that is provided with glyph data for pre-processing
---
--- When re-generating the text geometry, shaping will be
--- performed on the input string using the set font. The
--- result of shaping is a set of shaped glyphs. Shaped
--- glyphs are glyphs that have been positioned by the shaper
--- and whose script direction has also been determined.
---
--- Because multiple input codepoints can be merged into a
--- single glyph and single codepoints decomposed into multiple
--- glyphs, the shaper provides a way to map the shaping output
--- back to the input. When the input string is provided to
--- the shaper, a monotonically increasing character index is
--- attached to each input codepoint. If the input string
--- consists of 10 codepoints, the indices will be 0 to 9.
---
--- After shaping each shaped glyph will be assigned a
--- cluster value. These cluster values are derived from the
--- input indices that were provided to the shaper. Because
--- of the merging and decomposing that happens during shaping,
--- there isn't a 1 to 1 mapping between input indices and
--- output cluster values.
---
--- In order to set the glyph properties reliably, they have
--- to be set based on text segmentation boundaries such as
--- graphemes, words and sentences. See the corresponding
--- methods in `sf::String` that can check for these boundaries.
---
--- Once the input text segments to be pre-processed have
--- been determined, they have to be applied to the shaped
--- glyphs. When using character or grapheme cluster grouping
--- it is guaranteed that the resulting cluster values are
--- monotonic. This means that cluster values will not be
--- reordered beyond the bounds of the indices that were
--- provided with the input text.
---
--- What this means is that given a segment of text that
--- should e.g. be colored differently, if a beginning and
--- end index can be determined from the input codepoints,
--- these index boundaries can be used to select the clusters
--- of the shaped glyphs that correspond to the input segment
--- and thus whose color needs to be set.
---
--- Here is an example string with codepoint indices:
--- @code
--- I   l i k e   f l o w e r s ,   m u f f i n s   a n d   w a f f l e s .
--- 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3
--- 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
--- @endcode
--- After shaping, due to ligature merging of fl, ffi and ffl,
--- the output clusters might look like:
--- @code
--- I   l i k e   fl o w e r s ,   m u ffi n s   a n d   w a ffl e s .
--- 0 0 0 0 0 0 0 0  0 1 1 1 1 1 1 1 1 1   2 2 2 2 2 2 2 2 2 3   3 3 3
--- 0 1 2 3 4 5 6 7  9 0 1 2 3 4 5 6 7 8   1 2 3 4 5 6 7 8 9 0   3 4 5
--- @endcode
---
--- In order to e.g. color the word "muffins", the beginning
--- and end codepoint indices of the word have to be determined,
--- in this case 16 and 22. After shaping, any glyphs belonging
--- to the word "muffins" will have cluster values between and
--- including 16 and 22. In the example above the clusters
--- 16, 17, 18, 21 and 22 belong to the word "muffins".
--- Coloring the glyphs with those indices will result in the
--- word "muffins" being colored.
---
--- The same applies to "flowers" and "waffles" in the example
--- above.
---
--- Because merging and decomposition of codepoints cannot
--- happen beyond word boundaries, applying properties to
--- glyphs using the above method is safe when segmenting
--- based on words. As can be seen above it would not work
--- when attempting to apply a different property to the
--- single graphemes 'f', 'l' or 'i' since they can be
--- merged with neighbouring graphemes into a single glyph.
---
--- The opposite, decomposition, of the following input:
--- @code
--- I   f i n d   c l i c h é s   f u n n y .
--- 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1
--- 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0
--- @endcode
--- into glyphs would look like:
--- @code
--- I   f i n d   c l i c h e + s   f u n n y .
--- 0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1
--- 0 1 2 3 4 5 6 7 8 9 0 1 2 2 3 4 5 6 7 8 9 0
--- @endcode
--- The + at cluster 12 is a placeholder for the acute accent.
--- This can occur if the font provides the glyph for the accent
--- seperate from the base glyph e which also has the cluster
--- value 12. The codepoint é is thus decomposed into a base glyph
--- and a mark glyph. The cluster value of the mark in such a
--- decomposition will be identical to the base. Because of this,
--- the same procedure as demonstrated in the fist example can be
--- applied here as well.
---
--- The above examples are just simple examples of how to map
--- input codepoint indices to output cluster values. While
--- merging and decomposition are an exception in latin script,
--- they can occur very frequently in other scripts. The mapping
--- procedure described above will work for all scripts.
---
--- Once the boundaries of the cluster values whose properties to
--- modify have been determined, they can be used from within
--- the callable to set said properties on a glyph by glyph basis.
---
--- The callable will be called in the order in which glyph
--- geometry is generated. This does not always happen in
--- ascending cluster order such as in right-to-left text where
--- it happens in descending cluster order.
---
--- Be aware that while changing the character size per glyph
--- is not possible, changing its style or outline thickness
--- is. Doing this, however, might lead to slight inconsistencies
--- when the text bounds are computed at the end of the geometry
--- update process. The same applies to the italic style.
---
--- In contrast, changing the fill or outline color is safe
--- since they don't have any effect on the pixel coverage of
--- the glyph.
---
--- Setting the underlined and strikethrough styles per glyph
--- is technically possible but not yet implemented.
---
--- Note: Because text bounds are computed based on the
--- geometry, it is not safe or reliable to query the text bounds
--- from within this callable. If it is absolutely necessary
--- to make decisions within this callable based on text bounds,
--- multiple geometry updates will be necessary. The first
--- geometry update is run with the pre-processor set to
--- pass through data. Based on the first update the text bounds
--- can be queried and stored. The stored text bounds can then
--- be used in the second geometry update.
---@alias sf.Text.GlyphPreProcessor fun(shapedGlyph: sf.Text.ShapedGlyph, style: integer, fillColor: sf.Color, outlineColor: sf.Color, outlineThickness: number): {style: integer?, fillColor: sf.Color?, outlineColor: sf.Color?, outlineThickness: number?}|nil
--- @brief Vertex buffer storage for one or more 2D primitives
---@class sf.VertexBuffer : sf.Drawable
sf.VertexBuffer = sf.VertexBuffer or {}
--- @brief Construct a `VertexBuffer` with a specific `PrimitiveType` and usage specifier
---
--- Creates an empty vertex buffer and sets its primitive type
--- to @p type and usage to @p usage.
---
--- @param type  Type of primitive
--- @param usage Usage specifier
---@overload fun(type: sf.PrimitiveType): sf.VertexBuffer
---@overload fun(usage: sf.VertexBuffer.Usage): sf.VertexBuffer
---@overload fun(): sf.VertexBuffer
---@param type sf.PrimitiveType
---@param usage sf.VertexBuffer.Usage
---@return sf.VertexBuffer
function sf.VertexBuffer.new(type, usage) end
--- @brief Create the vertex buffer
---
--- Creates the vertex buffer and allocates enough graphics
--- memory to hold `vertexCount` vertices. Any previously
--- allocated memory is freed in the process.
---
--- In order to deallocate previously allocated memory pass 0
--- as `vertexCount`. Don't forget to recreate with a non-zero
--- value when graphics memory should be allocated again.
---
--- @param vertexCount Number of vertices worth of memory to allocate
---
--- @return `true` if creation was successful
---@type fun(self: sf.VertexBuffer, vertexCount: integer): boolean
sf.VertexBuffer.create = function() end
--- @brief Return the vertex count
---
--- @return Number of vertices in the vertex buffer
---@type fun(self: sf.VertexBuffer): integer
sf.VertexBuffer.getVertexCount = function() end
--- @brief Copy the contents of another buffer into this buffer
---
--- @param vertexBuffer Vertex buffer whose contents to copy into this vertex buffer
---
--- @return `true` if the copy was successful
---@overload fun(self: sf.VertexBuffer, vertices: any, offset: integer): boolean
---@overload fun(self: sf.VertexBuffer, vertices: any): boolean
---@param self sf.VertexBuffer
---@param vertexBuffer sf.VertexBuffer
---@return boolean
function sf.VertexBuffer.update(self, vertexBuffer) end
--- @brief Swap the contents of this vertex buffer with those of another
---
--- @param right Instance to swap with
---@type fun(self: sf.VertexBuffer, right: sf.VertexBuffer)
sf.VertexBuffer.swap = function() end
--- @brief Get the underlying OpenGL handle of the vertex buffer.
---
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return OpenGL handle of the vertex buffer or 0 if not yet created
---@type fun(self: sf.VertexBuffer): integer
sf.VertexBuffer.getNativeHandle = function() end
--- @brief Set the type of primitives to draw
---
--- This function defines how the vertices must be interpreted
--- when it's time to draw them.
---
--- The default primitive type is `sf::PrimitiveType::Points`.
---
--- @param type Type of primitive
---@type fun(self: sf.VertexBuffer, type: sf.PrimitiveType)
sf.VertexBuffer.setPrimitiveType = function() end
--- @brief Get the type of primitives drawn by the vertex buffer
---
--- @return Primitive type
---@type fun(self: sf.VertexBuffer): sf.PrimitiveType
sf.VertexBuffer.getPrimitiveType = function() end
--- @brief Set the usage specifier of this vertex buffer
---
--- This function provides a hint about how this vertex buffer is
--- going to be used in terms of data update frequency.
---
--- After changing the usage specifier, the vertex buffer has
--- to be updated with new data for the usage specifier to
--- take effect.
---
--- The default usage type is `sf::VertexBuffer::Usage::Stream`.
---
--- @param usage Usage specifier
---@type fun(self: sf.VertexBuffer, usage: sf.VertexBuffer.Usage)
sf.VertexBuffer.setUsage = function() end
--- @brief Get the usage specifier of this vertex buffer
---
--- @return Usage specifier
---@type fun(self: sf.VertexBuffer): sf.VertexBuffer.Usage
sf.VertexBuffer.getUsage = function() end
--- @brief Bind a vertex buffer for rendering
---
--- This function is not part of the graphics API, it mustn't be
--- used when drawing SFML entities. It must be used only if you
--- mix `sf::VertexBuffer` with OpenGL code.
---
--- @code
--- sf::VertexBuffer vb1, vb2;
--- ...
--- sf::VertexBuffer::bind(&vb1);
--- // draw OpenGL stuff that use vb1...
--- sf::VertexBuffer::bind(&vb2);
--- // draw OpenGL stuff that use vb2...
--- sf::VertexBuffer::bind(nullptr);
--- // draw OpenGL stuff that use no vertex buffer...
--- @endcode
---
--- @param vertexBuffer Pointer to the vertex buffer to bind, can be null to use no vertex buffer
---@type fun(vertexBuffer: sf.VertexBuffer)
sf.VertexBuffer.bind = function() end
--- @brief Tell whether or not the system supports vertex buffers
---
--- This function should always be called before using
--- the vertex buffer features. If it returns `false`, then
--- any attempt to use `sf::VertexBuffer` will fail.
---
--- @return `true` if vertex buffers are supported, `false` otherwise
---@type fun(): boolean
sf.VertexBuffer.isAvailable = function() end
--- @brief Usage specifiers
---
--- If data is going to be updated once or more every frame,
--- set the usage to Stream. If data is going to be set once
--- and used for a long time without being modified, set the
--- usage to Static. For everything else Dynamic should be a
--- good compromise.
---@class sf.VertexBuffer.Usage
--- Constantly changing data
---@field Stream sf.VertexBuffer.Usage
--- Occasionally changing data
---@field Dynamic sf.VertexBuffer.Usage
--- Rarely changing data
---@field Static sf.VertexBuffer.Usage
sf.VertexBuffer.Usage = sf.VertexBuffer.Usage or {}
--- @brief Swap the contents of one vertex buffer with those of another
---
--- @param left First instance to swap
--- @param right Second instance to swap
---@type fun(left: sf.VertexBuffer, right: sf.VertexBuffer)
sf.swap = function() end
--- @brief 2D camera that defines what region is shown on screen
---@class sf.View
sf.View = sf.View or {}
--- @brief Construct the view from its center and size
---
--- @param center Center of the zone to display
--- @param size   Size of zone to display
---@overload fun(rectangle: sf.FloatRect): sf.View
---@overload fun(): sf.View
---@param center sf.Vector2f
---@param size sf.Vector2f
---@return sf.View
function sf.View.new(center, size) end
--- @brief Set the center of the view
---
--- @param center New center
---
--- @see `setSize`, `getCenter`
---@type fun(self: sf.View, center: sf.Vector2f)
sf.View.setCenter = function() end
--- @brief Set the size of the view
---
--- @param size New size
---
--- @see `setCenter`, `getCenter`
---@type fun(self: sf.View, size: sf.Vector2f)
sf.View.setSize = function() end
--- @brief Set the orientation of the view
---
--- The default rotation of a view is 0 degree.
---
--- @param angle New angle
---
--- @see `getRotation`
---@type fun(self: sf.View, angle: sf.Angle)
sf.View.setRotation = function() end
--- @brief Set the target viewport
---
--- The viewport is the rectangle into which the contents of the
--- view are displayed, expressed as a factor (between 0 and 1)
--- of the size of the RenderTarget to which the view is applied.
--- For example, a view which takes the left side of the target would
--- be defined with `view.setViewport(sf::FloatRect({0.f, 0.f}, {0.5f, 1.f}))`.
--- By default, a view has a viewport which covers the entire target.
---
--- @param viewport New viewport rectangle
---
--- @see `getViewport`
---@type fun(self: sf.View, viewport: sf.FloatRect)
sf.View.setViewport = function() end
--- @brief Set the target scissor rectangle
---
--- The scissor rectangle, expressed as a factor (between 0 and 1) of
--- the RenderTarget, specifies the region of the RenderTarget whose
--- pixels are able to be modified by draw or clear operations.
--- Any pixels which lie outside of the scissor rectangle will
--- not be modified by draw or clear operations.
--- For example, a scissor rectangle which only allows modifications
--- to the right side of the target would be defined
--- with `view.setScissor(sf::FloatRect({0.5f, 0.f}, {0.5f, 1.f}))`.
--- By default, a view has a scissor rectangle which allows
--- modifications to the entire target. This is equivalent to
--- disabling the scissor test entirely. Passing the default
--- scissor rectangle to this function will also disable
--- scissor testing.
---
--- @param scissor New scissor rectangle
---
--- @see `getScissor`
---@type fun(self: sf.View, scissor: sf.FloatRect)
sf.View.setScissor = function() end
--- @brief Get the center of the view
---
--- @return Center of the view
---
--- @see `getSize`, `setCenter`
---@type fun(self: sf.View): sf.Vector2f
sf.View.getCenter = function() end
--- @brief Get the size of the view
---
--- @return Size of the view
---
--- @see `getCenter`, `setSize`
---@type fun(self: sf.View): sf.Vector2f
sf.View.getSize = function() end
--- @brief Get the current orientation of the view
---
--- @return Rotation angle of the view
---
--- @see `setRotation`
---@type fun(self: sf.View): sf.Angle
sf.View.getRotation = function() end
--- @brief Get the target viewport rectangle of the view
---
--- @return Viewport rectangle, expressed as a factor of the target size
---
--- @see `setViewport`
---@type fun(self: sf.View): sf.FloatRect
sf.View.getViewport = function() end
--- @brief Get the scissor rectangle of the view
---
--- @return Scissor rectangle, expressed as a factor of the target size
---
--- @see `setScissor`
---@type fun(self: sf.View): sf.FloatRect
sf.View.getScissor = function() end
--- @brief Move the view relative to its current position
---
--- @param offset Move offset
---
--- @see `setCenter`, `rotate`, `zoom`
---@type fun(self: sf.View, offset: sf.Vector2f)
sf.View.move = function() end
--- @brief Rotate the view relative to its current orientation
---
--- @param angle Angle to rotate
---
--- @see `setRotation`, `move`, `zoom`
---@type fun(self: sf.View, angle: sf.Angle)
sf.View.rotate = function() end
--- @brief Resize the view rectangle relative to its current size
---
--- Resizing the view simulates a zoom, as the zone displayed on
--- screen grows or shrinks.
--- @a factor is a multiplier:
--- @li 1 keeps the size unchanged
--- @li > 1 makes the view bigger (objects appear smaller)
--- @li < 1 makes the view smaller (objects appear bigger)
---
--- @param factor Zoom factor to apply
---
--- @see `setSize`, `move`, `rotate`
---@type fun(self: sf.View, factor: number)
sf.View.zoom = function() end
--- @brief Get the projection transform of the view
---
--- This function is meant for internal use only.
---
--- @return Projection transform defining the view
---
--- @see `getInverseTransform`
---@type fun(self: sf.View): sf.Transform
sf.View.getTransform = function() end
--- @brief Get the inverse projection transform of the view
---
--- This function is meant for internal use only.
---
--- @return Inverse of the projection transform defining the view
---
--- @see `getTransform`
---@type fun(self: sf.View): sf.Transform
sf.View.getInverseTransform = function() end
--- @brief Base class for all render targets (window, texture, ...)
---@class sf.RenderTarget
sf.RenderTarget = sf.RenderTarget or {}
--- @brief Clear the entire target with a single color and stencil value
---
--- The specified stencil value is truncated to the bit
--- width of the current stencil buffer.
---
--- @param color        Fill color to use to clear the render target
--- @param stencilValue Stencil value to clear to
---@overload fun(self: sf.RenderTarget, color: sf.Color)
---@overload fun(self: sf.RenderTarget)
---@param self sf.RenderTarget
---@param color sf.Color
---@param stencilValue sf.StencilValue
function sf.RenderTarget.clear(self, color, stencilValue) end
--- @brief Clear the stencil buffer to a specific value
---
--- The specified value is truncated to the bit width of
--- the current stencil buffer.
---
--- @param stencilValue Stencil value to clear to
---@type fun(self: sf.RenderTarget, stencilValue: sf.StencilValue)
sf.RenderTarget.clearStencil = function() end
--- @brief Change the current active view
---
--- The view is like a 2D camera, it controls which part of
--- the 2D scene is visible, and how it is viewed in the
--- render target.
--- The new view will affect everything that is drawn, until
--- another view is set.
--- The render target keeps its own copy of the view object,
--- so it is not necessary to keep the original one alive
--- after calling this function.
--- To restore the original view of the target, you can pass
--- the result of `getDefaultView()` to this function.
---
--- @param view New view to use
---
--- @see `getView`, `getDefaultView`
---@type fun(self: sf.RenderTarget, view: sf.View)
sf.RenderTarget.setView = function() end
--- @brief Get the view currently in use in the render target
---
--- @return The view object that is currently used
---
--- @see `setView`, `getDefaultView`
---@type fun(self: sf.RenderTarget): sf.View
sf.RenderTarget.getView = function() end
--- @brief Get the default view of the render target
---
--- The default view has the initial size of the render target,
--- and never changes after the target has been created.
---
--- @return The default view of the render target
---
--- @see `setView`, `getView`
---@type fun(self: sf.RenderTarget): sf.View
sf.RenderTarget.getDefaultView = function() end
--- @brief Get the viewport of a view, applied to this render target
---
--- The viewport is defined in the view as a ratio, this function
--- simply applies this ratio to the current dimensions of the
--- render target to calculate the pixels rectangle that the viewport
--- actually covers in the target.
---
--- @param view The view for which we want to compute the viewport
---
--- @return Viewport rectangle, expressed in pixels
---@type fun(self: sf.RenderTarget, view: sf.View): sf.IntRect
sf.RenderTarget.getViewport = function() end
--- @brief Get the scissor rectangle of a view, applied to this render target
---
--- The scissor rectangle is defined in the view as a ratio. This
--- function simply applies this ratio to the current dimensions
--- of the render target to calculate the pixels rectangle
--- that the scissor rectangle actually covers in the target.
---
--- @param view The view for which we want to compute the scissor rectangle
---
--- @return Scissor rectangle, expressed in pixels
---@type fun(self: sf.RenderTarget, view: sf.View): sf.IntRect
sf.RenderTarget.getScissor = function() end
--- @brief Convert a point from target coordinates to world coordinates
---
--- This function finds the 2D position that matches the
--- given pixel of the render target. In other words, it does
--- the inverse of what the graphics card does, to find the
--- initial position of a rendered pixel.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (10, 50) in your render target may map to the point
--- (150, 75) in your 2D world -- if the view is translated by (140, 25).
---
--- For render-windows, this function is typically used to find
--- which point (or object) is located below the mouse cursor.
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Pixel to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in "world" units
---
--- @see `mapCoordsToPixel`
---@overload fun(self: sf.RenderTarget, point: sf.Vector2i): sf.Vector2f
---@param self sf.RenderTarget
---@param point sf.Vector2i
---@param view sf.View
---@return sf.Vector2f
function sf.RenderTarget.mapPixelToCoords(self, point, view) end
--- @brief Convert a point from world coordinates to target coordinates
---
--- This function finds the pixel of the render target that matches
--- the given 2D point. In other words, it goes through the same process
--- as the graphics card, to compute the final position of a rendered point.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (150, 75) in your 2D world may map to the pixel
--- (10, 50) of your render target -- if the view is translated by (140, 25).
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Point to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in target coordinates (pixels)
---
--- @see `mapPixelToCoords`
---@overload fun(self: sf.RenderTarget, point: sf.Vector2f): sf.Vector2i
---@param self sf.RenderTarget
---@param point sf.Vector2f
---@param view sf.View
---@return sf.Vector2i
function sf.RenderTarget.mapCoordsToPixel(self, point, view) end
--- @brief Draw primitives defined by a vertex buffer
---
--- @param vertexBuffer Vertex buffer
--- @param firstVertex  Index of the first vertex to render
--- @param vertexCount  Number of vertices to render
--- @param states       Render states to use for drawing
---@overload fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)
---@overload fun(self: sf.RenderTarget, drawable: sf.Drawable, states: sf.RenderStates)
---@overload fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)
---@overload fun(self: sf.RenderTarget, drawable: sf.Drawable)
---@overload fun(self: sf.RenderTarget, vertexBuffer: sf.VertexBuffer)
---@overload fun(self: sf.RenderTarget, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)
---@overload fun(self: sf.RenderTarget, vertices: any, type: sf.PrimitiveType)
---@param self sf.RenderTarget
---@param vertexBuffer sf.VertexBuffer
---@param firstVertex integer
---@param vertexCount integer
---@param states sf.RenderStates
function sf.RenderTarget.draw(self, vertexBuffer, firstVertex, vertexCount, states) end
--- @brief Return the size of the rendering region of the target
---
--- @return Size in pixels
---@type fun(self: sf.RenderTarget): sf.Vector2u
sf.RenderTarget.getSize = function() end
--- @brief Tell if the render target will use sRGB encoding when drawing on it
---
--- @return `true` if the render target use sRGB encoding, `false` otherwise
---@type fun(self: sf.RenderTarget): boolean
sf.RenderTarget.isSrgb = function() end
--- @brief Activate or deactivate the render target for rendering
---
--- This function makes the render target's context current for
--- future OpenGL rendering operations (so you shouldn't care
--- about it if you're not doing direct OpenGL stuff).
--- A render target's context is active only on the current thread,
--- if you want to make it active on another thread you have
--- to deactivate it on the previous thread first if it was active.
--- Only one context can be current in a thread, so if you
--- want to draw OpenGL geometry to another render target
--- don't forget to activate it again. Activating a render
--- target will automatically deactivate the previously active
--- context (if any).
---
--- @param active `true` to activate, `false` to deactivate
---
--- @return `true` if operation was successful, `false` otherwise
---@overload fun(self: sf.RenderTarget): boolean
---@param self sf.RenderTarget
---@param active boolean
---@return boolean
function sf.RenderTarget.setActive(self, active) end
--- @brief Save the OpenGL render states modified by SFML
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering. Combined with popGLStates,
--- it ensures that:
--- @li SFML's internal states are not messed up by your OpenGL code
--- @li your OpenGL states are not modified by a call to a SFML function
---
--- More specifically, it must be used around code that
--- calls `draw` functions. Example:
--- @code
--- // OpenGL code here...
--- window.pushGLStates();
--- window.draw(...);
--- window.draw(...);
--- window.popGLStates();
--- // OpenGL code here...
--- @endcode
---
--- Note that this function is quite expensive: it saves the
--- program, textures, vertex attributes and the other OpenGL
--- states that SFML drawing can modify. State outside this set
--- is deliberately not covered.
--- It is provided for convenience, but the best results will
--- be achieved if you handle OpenGL states yourself (because
--- you know which states have really changed, and need to be
--- saved and restored). Take a look at the resetGLStates
--- function if you do so.
---
--- @see `popGLStates`
---@type fun(self: sf.RenderTarget)
sf.RenderTarget.pushGLStates = function() end
--- @brief Restore the previously saved OpenGL render states
---
--- See the description of `pushGLStates` to get a detailed
--- description of these functions.
---
--- @see `pushGLStates`
---@type fun(self: sf.RenderTarget)
sf.RenderTarget.popGLStates = function() end
--- @brief Reset the internal OpenGL states so that the target is ready for drawing
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering, if you choose not to use
--- `pushGLStates`/`popGLStates`. It makes sure that all OpenGL
--- states needed by SFML are set, so that subsequent `draw()`
--- calls will work as expected.
---
--- Example:
--- @code
--- // OpenGL code here...
--- window.resetGLStates();
--- window.draw(...);
--- window.draw(...);
--- // OpenGL code here...
--- @endcode
---@type fun(self: sf.RenderTarget)
sf.RenderTarget.resetGLStates = function() end
--- @brief Target for off-screen 2D rendering into a texture
---@class sf.RenderTexture : sf.RenderTarget
sf.RenderTexture = sf.RenderTexture or {}
--- @brief Construct a render-texture
---
--- The last parameter, `settings`, is useful if you want to enable
--- multi-sampling or use the render-texture for OpenGL rendering that
--- requires a depth or stencil buffer. Otherwise it is unnecessary, and
--- you should leave this parameter at its default value.
---
--- After creation, the contents of the render-texture are undefined.
--- Call `RenderTexture::clear` first to ensure a single color fill.
---
--- @param size     Width and height of the render-texture
--- @param settings Additional settings for the underlying OpenGL texture and context
---
--- @throws sf::Exception if creation was unsuccessful
---@overload fun(size: sf.Vector2u): sf.RenderTexture
---@overload fun(): sf.RenderTexture
---@param size sf.Vector2u
---@param settings sf.ContextSettings
---@return sf.RenderTexture
function sf.RenderTexture.new(size, settings) end
--- @brief Clear the entire target with a single color and stencil value
---
--- The specified stencil value is truncated to the bit
--- width of the current stencil buffer.
---
--- @param color        Fill color to use to clear the render target
--- @param stencilValue Stencil value to clear to
---@overload fun(self: sf.RenderTexture, color: sf.Color)
---@overload fun(self: sf.RenderTexture)
---@param self sf.RenderTexture
---@param color sf.Color
---@param stencilValue sf.StencilValue
function sf.RenderTexture.clear(self, color, stencilValue) end
--- @brief Clear the stencil buffer to a specific value
---
--- The specified value is truncated to the bit width of
--- the current stencil buffer.
---
--- @param stencilValue Stencil value to clear to
---@type fun(self: sf.RenderTexture, stencilValue: sf.StencilValue)
sf.RenderTexture.clearStencil = function() end
--- @brief Change the current active view
---
--- The view is like a 2D camera, it controls which part of
--- the 2D scene is visible, and how it is viewed in the
--- render target.
--- The new view will affect everything that is drawn, until
--- another view is set.
--- The render target keeps its own copy of the view object,
--- so it is not necessary to keep the original one alive
--- after calling this function.
--- To restore the original view of the target, you can pass
--- the result of `getDefaultView()` to this function.
---
--- @param view New view to use
---
--- @see `getView`, `getDefaultView`
---@type fun(self: sf.RenderTexture, view: sf.View)
sf.RenderTexture.setView = function() end
--- @brief Get the view currently in use in the render target
---
--- @return The view object that is currently used
---
--- @see `setView`, `getDefaultView`
---@type fun(self: sf.RenderTexture): sf.View
sf.RenderTexture.getView = function() end
--- @brief Get the default view of the render target
---
--- The default view has the initial size of the render target,
--- and never changes after the target has been created.
---
--- @return The default view of the render target
---
--- @see `setView`, `getView`
---@type fun(self: sf.RenderTexture): sf.View
sf.RenderTexture.getDefaultView = function() end
--- @brief Get the viewport of a view, applied to this render target
---
--- The viewport is defined in the view as a ratio, this function
--- simply applies this ratio to the current dimensions of the
--- render target to calculate the pixels rectangle that the viewport
--- actually covers in the target.
---
--- @param view The view for which we want to compute the viewport
---
--- @return Viewport rectangle, expressed in pixels
---@type fun(self: sf.RenderTexture, view: sf.View): sf.IntRect
sf.RenderTexture.getViewport = function() end
--- @brief Get the scissor rectangle of a view, applied to this render target
---
--- The scissor rectangle is defined in the view as a ratio. This
--- function simply applies this ratio to the current dimensions
--- of the render target to calculate the pixels rectangle
--- that the scissor rectangle actually covers in the target.
---
--- @param view The view for which we want to compute the scissor rectangle
---
--- @return Scissor rectangle, expressed in pixels
---@type fun(self: sf.RenderTexture, view: sf.View): sf.IntRect
sf.RenderTexture.getScissor = function() end
--- @brief Convert a point from target coordinates to world coordinates
---
--- This function finds the 2D position that matches the
--- given pixel of the render target. In other words, it does
--- the inverse of what the graphics card does, to find the
--- initial position of a rendered pixel.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (10, 50) in your render target may map to the point
--- (150, 75) in your 2D world -- if the view is translated by (140, 25).
---
--- For render-windows, this function is typically used to find
--- which point (or object) is located below the mouse cursor.
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Pixel to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in "world" units
---
--- @see `mapCoordsToPixel`
---@overload fun(self: sf.RenderTexture, point: sf.Vector2i): sf.Vector2f
---@param self sf.RenderTexture
---@param point sf.Vector2i
---@param view sf.View
---@return sf.Vector2f
function sf.RenderTexture.mapPixelToCoords(self, point, view) end
--- @brief Convert a point from world coordinates to target coordinates
---
--- This function finds the pixel of the render target that matches
--- the given 2D point. In other words, it goes through the same process
--- as the graphics card, to compute the final position of a rendered point.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (150, 75) in your 2D world may map to the pixel
--- (10, 50) of your render target -- if the view is translated by (140, 25).
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Point to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in target coordinates (pixels)
---
--- @see `mapPixelToCoords`
---@overload fun(self: sf.RenderTexture, point: sf.Vector2f): sf.Vector2i
---@param self sf.RenderTexture
---@param point sf.Vector2f
---@param view sf.View
---@return sf.Vector2i
function sf.RenderTexture.mapCoordsToPixel(self, point, view) end
--- @brief Draw primitives defined by a vertex buffer
---
--- @param vertexBuffer Vertex buffer
--- @param firstVertex  Index of the first vertex to render
--- @param vertexCount  Number of vertices to render
--- @param states       Render states to use for drawing
---@overload fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)
---@overload fun(self: sf.RenderTexture, drawable: sf.Drawable, states: sf.RenderStates)
---@overload fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)
---@overload fun(self: sf.RenderTexture, drawable: sf.Drawable)
---@overload fun(self: sf.RenderTexture, vertexBuffer: sf.VertexBuffer)
---@overload fun(self: sf.RenderTexture, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)
---@overload fun(self: sf.RenderTexture, vertices: any, type: sf.PrimitiveType)
---@param self sf.RenderTexture
---@param vertexBuffer sf.VertexBuffer
---@param firstVertex integer
---@param vertexCount integer
---@param states sf.RenderStates
function sf.RenderTexture.draw(self, vertexBuffer, firstVertex, vertexCount, states) end
--- @brief Return the size of the rendering region of the texture
---
--- The returned value is the size that you passed to
--- the create function.
---
--- @return Size in pixels
---@type fun(self: sf.RenderTexture): sf.Vector2u
sf.RenderTexture.getSize = function() end
--- @brief Tell if the render-texture will use sRGB encoding when drawing on it
---
--- You can request sRGB encoding for a render-texture
--- by having the sRgbCapable flag set for the context parameter of `create()` method
---
--- @return `true` if the render-texture use sRGB encoding, `false` otherwise
---@type fun(self: sf.RenderTexture): boolean
sf.RenderTexture.isSrgb = function() end
--- @brief Activate or deactivate the render-texture for rendering
---
--- This function makes the render-texture's context current for
--- future OpenGL rendering operations (so you shouldn't care
--- about it if you're not doing direct OpenGL stuff).
--- Only one context can be current in a thread, so if you
--- want to draw OpenGL geometry to another render target
--- (like a RenderWindow) don't forget to activate it again.
---
--- @param active `true` to activate, `false` to deactivate
---
--- @return `true` if operation was successful, `false` otherwise
---@overload fun(self: sf.RenderTexture): boolean
---@param self sf.RenderTexture
---@param active boolean
---@return boolean
function sf.RenderTexture.setActive(self, active) end
--- @brief Save the OpenGL render states modified by SFML
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering. Combined with popGLStates,
--- it ensures that:
--- @li SFML's internal states are not messed up by your OpenGL code
--- @li your OpenGL states are not modified by a call to a SFML function
---
--- More specifically, it must be used around code that
--- calls `draw` functions. Example:
--- @code
--- // OpenGL code here...
--- window.pushGLStates();
--- window.draw(...);
--- window.draw(...);
--- window.popGLStates();
--- // OpenGL code here...
--- @endcode
---
--- Note that this function is quite expensive: it saves the
--- program, textures, vertex attributes and the other OpenGL
--- states that SFML drawing can modify. State outside this set
--- is deliberately not covered.
--- It is provided for convenience, but the best results will
--- be achieved if you handle OpenGL states yourself (because
--- you know which states have really changed, and need to be
--- saved and restored). Take a look at the resetGLStates
--- function if you do so.
---
--- @see `popGLStates`
---@type fun(self: sf.RenderTexture)
sf.RenderTexture.pushGLStates = function() end
--- @brief Restore the previously saved OpenGL render states
---
--- See the description of `pushGLStates` to get a detailed
--- description of these functions.
---
--- @see `pushGLStates`
---@type fun(self: sf.RenderTexture)
sf.RenderTexture.popGLStates = function() end
--- @brief Reset the internal OpenGL states so that the target is ready for drawing
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering, if you choose not to use
--- `pushGLStates`/`popGLStates`. It makes sure that all OpenGL
--- states needed by SFML are set, so that subsequent `draw()`
--- calls will work as expected.
---
--- Example:
--- @code
--- // OpenGL code here...
--- window.resetGLStates();
--- window.draw(...);
--- window.draw(...);
--- // OpenGL code here...
--- @endcode
---@type fun(self: sf.RenderTexture)
sf.RenderTexture.resetGLStates = function() end
--- @brief Resize the render-texture
---
--- The last parameter, `settings`, is useful if you want to enable
--- multi-sampling or use the render-texture for OpenGL rendering that
--- requires a depth or stencil buffer. Otherwise it is unnecessary, and
--- you should leave this parameter at its default value.
---
--- After resizing, the contents of the render-texture are undefined.
--- Call `RenderTexture::clear` first to ensure a single color fill.
---
--- @param size     Width and height of the render-texture
--- @param settings Additional settings for the underlying OpenGL texture and context
---
--- @return `true` if resizing has been successful, `false` if it failed
---@overload fun(self: sf.RenderTexture, size: sf.Vector2u): boolean
---@param self sf.RenderTexture
---@param size sf.Vector2u
---@param settings sf.ContextSettings
---@return boolean
function sf.RenderTexture.resize(self, size, settings) end
--- @brief Get the maximum anti-aliasing level supported by the system
---
--- @return The maximum anti-aliasing level supported by the system
---@type fun(): integer
sf.RenderTexture.getMaximumAntiAliasingLevel = function() end
--- @brief Enable or disable texture smoothing
---
--- This function is similar to `Texture::setSmooth`.
--- This parameter is disabled by default.
---
--- @param smooth `true` to enable smoothing, `false` to disable it
---
--- @see `isSmooth`
---@type fun(self: sf.RenderTexture, smooth: boolean)
sf.RenderTexture.setSmooth = function() end
--- @brief Tell whether the smooth filtering is enabled or not
---
--- @return `true` if texture smoothing is enabled
---
--- @see `setSmooth`
---@type fun(self: sf.RenderTexture): boolean
sf.RenderTexture.isSmooth = function() end
--- @brief Enable or disable texture repeating
---
--- This function is similar to `Texture::setRepeated`.
--- This parameter is disabled by default.
---
--- @param repeated `true` to enable repeating, `false` to disable it
---
--- @see `isRepeated`
---@type fun(self: sf.RenderTexture, repeated: boolean)
sf.RenderTexture.setRepeated = function() end
--- @brief Tell whether the texture is repeated or not
---
--- @return `true` if texture is repeated
---
--- @see `setRepeated`
---@type fun(self: sf.RenderTexture): boolean
sf.RenderTexture.isRepeated = function() end
--- @brief Generate a mipmap using the current texture data
---
--- This function is similar to `Texture::generateMipmap` and operates
--- on the texture used as the target for drawing.
--- Be aware that any draw operation may modify the base level image data.
--- For this reason, calling this function only makes sense after all
--- drawing is completed and display has been called. Not calling display
--- after subsequent drawing will lead to undefined behavior if a mipmap
--- had been previously generated.
---
--- @return `true` if mipmap generation was successful, `false` if unsuccessful
---@type fun(self: sf.RenderTexture): boolean
sf.RenderTexture.generateMipmap = function() end
--- @brief Update the contents of the target texture
---
--- This function updates the target texture with what
--- has been drawn so far. Like for windows, calling this
--- function is mandatory at the end of rendering. Not calling
--- it may leave the texture in an undefined state.
---@type fun(self: sf.RenderTexture)
sf.RenderTexture.display = function() end
--- @brief Get a read-only reference to the target texture
---
--- After drawing to the render-texture and calling Display,
--- you can retrieve the updated texture using this function,
--- and draw it using a sprite (for example).
--- The internal `sf::Texture` of a render-texture is always the
--- same instance, so that it is possible to call this function
--- once and keep a reference to the texture even after it is
--- modified.
---
--- @return Const reference to the texture
---@type fun(self: sf.RenderTexture): sf.Texture
sf.RenderTexture.getTexture = function() end
--- @brief Window that can serve as a target for 2D drawing
---@class sf.RenderWindow : sf.Window, sf.WindowBase, sf.RenderTarget
sf.RenderWindow = sf.RenderWindow or {}
--- @brief Construct a new window
---
--- This constructor creates the window with the size and pixel
--- depth defined in `mode`. An optional style can be passed to
--- customize the look and behavior of the window (borders,
--- title bar, resizable, closable, ...).
---
--- The last parameter is an optional structure specifying
--- advanced OpenGL context settings such as anti-aliasing,
--- depth-buffer bits, etc. You shouldn't care about these
--- parameters for a regular usage of the graphics module.
---
--- @param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title    Title of the window
--- @param style    %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state    %Window state
--- @param settings Additional settings for the underlying OpenGL context
---@overload fun(mode: sf.VideoMode, title: string, style: integer, state: sf.State): sf.RenderWindow
---@overload fun(mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings): sf.RenderWindow
---@overload fun(mode: sf.VideoMode, title: string, style: integer): sf.RenderWindow
---@overload fun(mode: sf.VideoMode, title: string, state: sf.State): sf.RenderWindow
---@overload fun(mode: sf.VideoMode, title: string): sf.RenderWindow
---@overload fun(handle: nil, settings: sf.ContextSettings): sf.RenderWindow
---@overload fun(handle: nil): sf.RenderWindow
---@overload fun(): sf.RenderWindow
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
---@param settings sf.ContextSettings
---@return sf.RenderWindow
function sf.RenderWindow.new(mode, title, style, state, settings) end
--- @brief Create (or recreate) the window
---
--- If the window was already created, it closes it first.
--- If `state` is `State::Fullscreen`, then `mode` must be
--- a valid video mode.
---
--- The last parameter is a structure specifying advanced OpenGL
--- context settings such as anti-aliasing, depth-buffer bits, etc.
---
--- @param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
--- @param title    Title of the window
--- @param style    %Window style, a bitwise OR combination of `sf::Style` enumerators
--- @param state    %Window state
--- @param settings Additional settings for the underlying OpenGL context
---@overload fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer, state: sf.State)
---@overload fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State, settings: sf.ContextSettings)
---@overload fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, style: integer)
---@overload fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string, state: sf.State)
---@overload fun(self: sf.RenderWindow, mode: sf.VideoMode, title: string)
---@overload fun(self: sf.RenderWindow, handle: nil, settings: sf.ContextSettings)
---@overload fun(self: sf.RenderWindow, handle: nil)
---@param self sf.RenderWindow
---@param mode sf.VideoMode
---@param title string
---@param style integer
---@param state sf.State
---@param settings sf.ContextSettings
function sf.RenderWindow.create(self, mode, title, style, state, settings) end
--- @brief Close the window and destroy all the attached resources
---
--- After calling this function, the `sf::Window` instance remains
--- valid and you can call `create()` to recreate the window.
--- All other functions such as `pollEvent()` or `display()` will
--- still work (i.e. you don't have to test `isOpen()` every time),
--- and will have no effect on closed windows.
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.close = function() end
--- @brief Tell whether or not the window is open
---
--- This function returns whether or not the window exists.
--- Note that a hidden window (`setVisible(false)`) is open
--- (therefore this function would return `true`).
---
--- @return `true` if the window is open, `false` if it has been closed
---@type fun(self: sf.RenderWindow): boolean
sf.RenderWindow.isOpen = function() end
--- @brief Pop the next event from the front of the FIFO event queue, if any, and return it
---
--- This function is not blocking: if there's no pending event then
--- it will return a `std::nullopt`. Note that more than one event
--- may be present in the event queue, thus you should always call
--- this function in a loop to make sure that you process every
--- pending event.
--- @code
--- while (const std::optional event = window.pollEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @return The event, otherwise `std::nullopt` if no events are pending
---
--- @see `waitEvent`, `handleEvents`
---@type fun(self: sf.RenderWindow): sf.Event|nil
sf.RenderWindow.pollEvent = function() end
--- @brief Wait for an event and return it
---
--- This function is blocking: if there's no pending event then
--- it will wait until an event is received or until the provided
--- timeout elapses. Only if an error or a timeout occurs the
--- returned event will be `std::nullopt`.
--- This function is typically used when you have a thread that is
--- dedicated to events handling: you want to make this thread sleep
--- as long as no new event is received.
--- @code
--- while (const std::optional event = window.waitEvent())
--- {
--- // process event...
--- }
--- @endcode
---
--- @param timeout Maximum time to wait (`Time::Zero` for infinite)
---
--- @return The event, otherwise `std::nullopt` on timeout or if window was closed
---
--- @see `pollEvent`, `handleEvents`
---@overload fun(self: sf.RenderWindow): sf.Event|nil
---@param self sf.RenderWindow
---@param timeout sf.Time
---@return sf.Event|nil
function sf.RenderWindow.waitEvent(self, timeout) end
--- @brief Get the position of the window
---
--- @return Position of the window, in pixels
---
--- @see `setPosition`
---@type fun(self: sf.RenderWindow): sf.Vector2i
sf.RenderWindow.getPosition = function() end
--- @brief Change the position of the window on screen
---
--- This function only works for top-level windows
--- (i.e. it will be ignored for windows created from
--- the handle of a child window/control).
---
--- @param position New position, in pixels
---
--- @see `getPosition`
---@type fun(self: sf.RenderWindow, position: sf.Vector2i)
sf.RenderWindow.setPosition = function() end
--- @brief Get the size of the rendering region of the window
---
--- The size doesn't include the titlebar and borders
--- of the window.
---
--- @return Size in pixels
---@type fun(self: sf.RenderWindow): sf.Vector2u
sf.RenderWindow.getSize = function() end
--- @brief Change the size of the rendering region of the window
---
--- @param size New size, in pixels
---
--- @see `getSize`
---@type fun(self: sf.RenderWindow, size: sf.Vector2u)
sf.RenderWindow.setSize = function() end
--- @brief Set the minimum window rendering region size
---
--- Pass `std::nullopt` to unset the minimum size
---
--- @param minimumSize New minimum size, in pixels
---@type fun(self: sf.RenderWindow, minimumSize: sf.Vector2u|nil)
sf.RenderWindow.setMinimumSize = function() end
--- @brief Set the maximum window rendering region size
---
--- Pass `std::nullopt` to unset the maximum size
---
--- @param maximumSize New maximum size, in pixels
---@type fun(self: sf.RenderWindow, maximumSize: sf.Vector2u|nil)
sf.RenderWindow.setMaximumSize = function() end
--- @brief Change the title of the window
---
--- @param title New title
---
--- @see `setIcon`
---@type fun(self: sf.RenderWindow, title: string)
sf.RenderWindow.setTitle = function() end
--- @brief Change the window's icon
---
--- The OS default icon is used by default.
---
--- @param icon Image to use as the icon. The image is copied,
--- so you need not keep the source alive after
--- calling this function.
---@overload fun(self: sf.RenderWindow, size: sf.Vector2u, pixels: any)
---@param self sf.RenderWindow
---@param icon sf.Image
function sf.RenderWindow.setIcon(self, icon) end
--- @brief Show or hide the window
---
--- The window is shown by default.
---
--- @param visible `true` to show the window, `false` to hide it
---@type fun(self: sf.RenderWindow, visible: boolean)
sf.RenderWindow.setVisible = function() end
--- @brief Show or hide the mouse cursor
---
--- The mouse cursor is visible by default.
---
--- @warning On Windows, this function needs to be called from the
--- thread that created the window.
---
--- @param visible `true` to show the mouse cursor, `false` to hide it
---@type fun(self: sf.RenderWindow, visible: boolean)
sf.RenderWindow.setMouseCursorVisible = function() end
--- @brief Grab or release the mouse cursor
---
--- If set, grabs the mouse cursor inside this window's client
--- area so it may no longer be moved outside its bounds.
--- Note that grabbing is only active while the window has
--- focus.
---
--- @param grabbed `true` to enable, `false` to disable
---@type fun(self: sf.RenderWindow, grabbed: boolean)
sf.RenderWindow.setMouseCursorGrabbed = function() end
--- @brief Set the displayed cursor to a native system cursor
---
--- Upon window creation, the arrow cursor is used by default.
---
--- @warning The cursor must not be destroyed while in use by
--- the window.
---
--- @warning Features related to Cursor are not supported on
--- iOS and Android.
---
--- @param cursor Native system cursor type to display
---
--- @see `sf::Cursor::createFromSystem`, `sf::Cursor::createFromPixels`
---@type fun(self: sf.RenderWindow, cursor: sf.Cursor)
sf.RenderWindow.setMouseCursor = function() end
--- @brief Enable or disable automatic key-repeat
---
--- If key repeat is enabled, you will receive repeated
--- KeyPressed events while keeping a key pressed. If it is disabled,
--- you will only get a single event when the key is pressed.
---
--- Key repeat is enabled by default.
---
--- @param enabled `true` to enable, `false` to disable
---@type fun(self: sf.RenderWindow, enabled: boolean)
sf.RenderWindow.setKeyRepeatEnabled = function() end
--- @brief Change the joystick threshold
---
--- The joystick threshold is the value below which
--- no JoystickMoved event will be generated.
---
--- The threshold value is 0.1 by default.
---
--- @param threshold New threshold, in the range [0, 100]
---@type fun(self: sf.RenderWindow, threshold: number)
sf.RenderWindow.setJoystickThreshold = function() end
--- @brief Request the current window to be made the active
--- foreground window
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or mouse events.
--- If a window requests focus, it only hints to the operating
--- system, that it would like to be focused. The operating system
--- is free to deny the request.
--- This is not to be confused with `setActive()`.
---
--- @see `hasFocus`
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.requestFocus = function() end
--- @brief Check whether the window has the input focus
---
--- At any given time, only one window may have the input focus
--- to receive input events such as keystrokes or most mouse
--- events.
---
--- @return `true` if window has focus, `false` otherwise
--- @see `requestFocus`
---@type fun(self: sf.RenderWindow): boolean
sf.RenderWindow.hasFocus = function() end
--- @brief Get the OS-specific handle of the window
---
--- The type of the returned handle is `sf::WindowHandle`,
--- which is a type alias to the handle type defined by the OS.
--- You shouldn't need to use this function, unless you have
--- very specific stuff to implement that SFML doesn't support,
--- or implement a temporary workaround until a bug is fixed.
---
--- @return System handle of the window
---@type fun(self: sf.RenderWindow): sf.WindowHandle
sf.RenderWindow.getNativeHandle = function() end
--- @brief Get the settings of the OpenGL context of the window
---
--- Note that these settings may be different from what was
--- passed to the constructor or the `create()` function,
--- if one or more settings were not supported. In this case,
--- SFML chose the closest match.
---
--- @return Structure containing the OpenGL context settings
---@type fun(self: sf.RenderWindow): sf.ContextSettings
sf.RenderWindow.getSettings = function() end
--- @brief Enable or disable vertical synchronization
---
--- Activating vertical synchronization will limit the number
--- of frames displayed to the refresh rate of the monitor.
--- This can avoid some visual artifacts, and limit the framerate
--- to a good value (but not constant across different computers).
---
--- Vertical synchronization is disabled by default.
---
--- @param enabled `true` to enable v-sync, `false` to deactivate it
---@type fun(self: sf.RenderWindow, enabled: boolean)
sf.RenderWindow.setVerticalSyncEnabled = function() end
--- @brief Limit the framerate to a maximum fixed frequency
---
--- If a limit is set, the window will use a small delay after
--- each call to `display()` to ensure that the current frame
--- lasted long enough to match the framerate limit.
--- SFML will try to match the given limit as much as it can,
--- but since it internally uses `sf::sleep`, whose precision
--- depends on the underlying OS, the results may be a little
--- imprecise as well (for example, you can get 65 FPS when
--- requesting 60).
---
--- @param limit Framerate limit, in frames per seconds (use 0 to disable limit)
---@type fun(self: sf.RenderWindow, limit: integer)
sf.RenderWindow.setFramerateLimit = function() end
--- @brief Activate or deactivate the window as the current target
--- for OpenGL rendering
---
--- A window is active only on the current thread, if you want to
--- make it active on another thread you have to deactivate it
--- on the previous thread first if it was active.
--- Only one window can be active on a thread at a time, thus
--- the window previously active (if any) automatically gets deactivated.
--- This is not to be confused with `requestFocus()`.
---
--- @param active `true` to activate, `false` to deactivate
---
--- @return `true` if operation was successful, `false` otherwise
---@overload fun(self: sf.RenderWindow): boolean
---@param self sf.RenderWindow
---@param active boolean
---@return boolean
function sf.RenderWindow.setActive(self, active) end
--- @brief Display on screen what has been rendered to the window so far
---
--- This function is typically called after all OpenGL rendering
--- has been done for the current frame, in order to show
--- it on screen.
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.display = function() end
--- @brief Clear the entire target with a single color and stencil value
---
--- The specified stencil value is truncated to the bit
--- width of the current stencil buffer.
---
--- @param color        Fill color to use to clear the render target
--- @param stencilValue Stencil value to clear to
---@overload fun(self: sf.RenderWindow, color: sf.Color)
---@overload fun(self: sf.RenderWindow)
---@param self sf.RenderWindow
---@param color sf.Color
---@param stencilValue sf.StencilValue
function sf.RenderWindow.clear(self, color, stencilValue) end
--- @brief Clear the stencil buffer to a specific value
---
--- The specified value is truncated to the bit width of
--- the current stencil buffer.
---
--- @param stencilValue Stencil value to clear to
---@type fun(self: sf.RenderWindow, stencilValue: sf.StencilValue)
sf.RenderWindow.clearStencil = function() end
--- @brief Change the current active view
---
--- The view is like a 2D camera, it controls which part of
--- the 2D scene is visible, and how it is viewed in the
--- render target.
--- The new view will affect everything that is drawn, until
--- another view is set.
--- The render target keeps its own copy of the view object,
--- so it is not necessary to keep the original one alive
--- after calling this function.
--- To restore the original view of the target, you can pass
--- the result of `getDefaultView()` to this function.
---
--- @param view New view to use
---
--- @see `getView`, `getDefaultView`
---@type fun(self: sf.RenderWindow, view: sf.View)
sf.RenderWindow.setView = function() end
--- @brief Get the view currently in use in the render target
---
--- @return The view object that is currently used
---
--- @see `setView`, `getDefaultView`
---@type fun(self: sf.RenderWindow): sf.View
sf.RenderWindow.getView = function() end
--- @brief Get the default view of the render target
---
--- The default view has the initial size of the render target,
--- and never changes after the target has been created.
---
--- @return The default view of the render target
---
--- @see `setView`, `getView`
---@type fun(self: sf.RenderWindow): sf.View
sf.RenderWindow.getDefaultView = function() end
--- @brief Get the viewport of a view, applied to this render target
---
--- The viewport is defined in the view as a ratio, this function
--- simply applies this ratio to the current dimensions of the
--- render target to calculate the pixels rectangle that the viewport
--- actually covers in the target.
---
--- @param view The view for which we want to compute the viewport
---
--- @return Viewport rectangle, expressed in pixels
---@type fun(self: sf.RenderWindow, view: sf.View): sf.IntRect
sf.RenderWindow.getViewport = function() end
--- @brief Get the scissor rectangle of a view, applied to this render target
---
--- The scissor rectangle is defined in the view as a ratio. This
--- function simply applies this ratio to the current dimensions
--- of the render target to calculate the pixels rectangle
--- that the scissor rectangle actually covers in the target.
---
--- @param view The view for which we want to compute the scissor rectangle
---
--- @return Scissor rectangle, expressed in pixels
---@type fun(self: sf.RenderWindow, view: sf.View): sf.IntRect
sf.RenderWindow.getScissor = function() end
--- @brief Convert a point from target coordinates to world coordinates
---
--- This function finds the 2D position that matches the
--- given pixel of the render target. In other words, it does
--- the inverse of what the graphics card does, to find the
--- initial position of a rendered pixel.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (10, 50) in your render target may map to the point
--- (150, 75) in your 2D world -- if the view is translated by (140, 25).
---
--- For render-windows, this function is typically used to find
--- which point (or object) is located below the mouse cursor.
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Pixel to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in "world" units
---
--- @see `mapCoordsToPixel`
---@overload fun(self: sf.RenderWindow, point: sf.Vector2i): sf.Vector2f
---@param self sf.RenderWindow
---@param point sf.Vector2i
---@param view sf.View
---@return sf.Vector2f
function sf.RenderWindow.mapPixelToCoords(self, point, view) end
--- @brief Convert a point from world coordinates to target coordinates
---
--- This function finds the pixel of the render target that matches
--- the given 2D point. In other words, it goes through the same process
--- as the graphics card, to compute the final position of a rendered point.
---
--- Initially, both coordinate systems (world units and target pixels)
--- match perfectly. But if you define a custom view or resize your
--- render target, this assertion is not `true` anymore, i.e. a point
--- located at (150, 75) in your 2D world may map to the pixel
--- (10, 50) of your render target -- if the view is translated by (140, 25).
---
--- This version uses a custom view for calculations, see the other
--- overload of the function if you want to use the current view of the
--- render target.
---
--- @param point Point to convert
--- @param view The view to use for converting the point
---
--- @return The converted point, in target coordinates (pixels)
---
--- @see `mapPixelToCoords`
---@overload fun(self: sf.RenderWindow, point: sf.Vector2f): sf.Vector2i
---@param self sf.RenderWindow
---@param point sf.Vector2f
---@param view sf.View
---@return sf.Vector2i
function sf.RenderWindow.mapCoordsToPixel(self, point, view) end
--- @brief Draw primitives defined by a vertex buffer
---
--- @param vertexBuffer Vertex buffer
--- @param firstVertex  Index of the first vertex to render
--- @param vertexCount  Number of vertices to render
--- @param states       Render states to use for drawing
---@overload fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, firstVertex: integer, vertexCount: integer)
---@overload fun(self: sf.RenderWindow, drawable: sf.Drawable, states: sf.RenderStates)
---@overload fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer, states: sf.RenderStates)
---@overload fun(self: sf.RenderWindow, drawable: sf.Drawable)
---@overload fun(self: sf.RenderWindow, vertexBuffer: sf.VertexBuffer)
---@overload fun(self: sf.RenderWindow, vertices: any, type: sf.PrimitiveType, states: sf.RenderStates)
---@overload fun(self: sf.RenderWindow, vertices: any, type: sf.PrimitiveType)
---@param self sf.RenderWindow
---@param vertexBuffer sf.VertexBuffer
---@param firstVertex integer
---@param vertexCount integer
---@param states sf.RenderStates
function sf.RenderWindow.draw(self, vertexBuffer, firstVertex, vertexCount, states) end
--- @brief Tell if the window will use sRGB encoding when drawing on it
---
--- You can request sRGB encoding for a window by having the sRgbCapable flag set in the `ContextSettings`
---
--- @return `true` if the window use sRGB encoding, `false` otherwise
---@type fun(self: sf.RenderWindow): boolean
sf.RenderWindow.isSrgb = function() end
--- @brief Save the OpenGL render states modified by SFML
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering. Combined with popGLStates,
--- it ensures that:
--- @li SFML's internal states are not messed up by your OpenGL code
--- @li your OpenGL states are not modified by a call to a SFML function
---
--- More specifically, it must be used around code that
--- calls `draw` functions. Example:
--- @code
--- // OpenGL code here...
--- window.pushGLStates();
--- window.draw(...);
--- window.draw(...);
--- window.popGLStates();
--- // OpenGL code here...
--- @endcode
---
--- Note that this function is quite expensive: it saves the
--- program, textures, vertex attributes and the other OpenGL
--- states that SFML drawing can modify. State outside this set
--- is deliberately not covered.
--- It is provided for convenience, but the best results will
--- be achieved if you handle OpenGL states yourself (because
--- you know which states have really changed, and need to be
--- saved and restored). Take a look at the resetGLStates
--- function if you do so.
---
--- @see `popGLStates`
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.pushGLStates = function() end
--- @brief Restore the previously saved OpenGL render states
---
--- See the description of `pushGLStates` to get a detailed
--- description of these functions.
---
--- @see `pushGLStates`
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.popGLStates = function() end
--- @brief Reset the internal OpenGL states so that the target is ready for drawing
---
--- This function can be used when you mix SFML drawing
--- and direct OpenGL rendering, if you choose not to use
--- `pushGLStates`/`popGLStates`. It makes sure that all OpenGL
--- states needed by SFML are set, so that subsequent `draw()`
--- calls will work as expected.
---
--- Example:
--- @code
--- // OpenGL code here...
--- window.resetGLStates();
--- window.draw(...);
--- window.draw(...);
--- // OpenGL code here...
--- @endcode
---@type fun(self: sf.RenderWindow)
sf.RenderWindow.resetGLStates = function() end
--- @brief Structure defining the properties of a directional cone
---
--- Sounds will play at gain 1 when they are positioned
--- within the inner angle of the cone. Sounds will play
--- at `outerGain` when they are positioned outside the
--- outer angle of the cone. The gain declines linearly
--- from 1 to `outerGain` as the sound moves from the inner
--- angle to the outer angle.
---@class sf.Listener.Cone
--- Inner angle
---@field innerAngle sf.Angle
--- Outer angle
---@field outerAngle sf.Angle
--- Outer gain
---@field outerGain number
sf.Listener = sf.Listener or {}
sf.Listener.Cone = sf.Listener.Cone or {}
---@type fun(): sf.Listener.Cone
sf.Listener.Cone.new = function() end
--- @brief Change the global volume of all the sounds and musics
---
--- `volume` is a number between 0 and 100; it is combined
--- with the individual volume of each sound / music.
--- The default value for the volume is 100 (maximum).
---
--- @param volume New global volume, in the range [0, 100]
---
--- @see `getGlobalVolume`
---@type fun(volume: number)
sf.Listener.setGlobalVolume = function() end
--- @brief Get the current value of the global volume
---
--- @return Current global volume, in the range [0, 100]
---
--- @see `setGlobalVolume`
---@type fun(): number
sf.Listener.getGlobalVolume = function() end
--- @brief Set the position of the listener in the scene
---
--- The default listener's position is (0, 0, 0).
---
--- @param position New listener's position
---
--- @see `getPosition`, `setDirection`
---@type fun(position: sf.Vector3f)
sf.Listener.setPosition = function() end
--- @brief Get the current position of the listener in the scene
---
--- @return Listener's position
---
--- @see `setPosition`
---@type fun(): sf.Vector3f
sf.Listener.getPosition = function() end
--- @brief Set the forward vector of the listener in the scene
---
--- The direction (also called "at vector") is the vector
--- pointing forward from the listener's perspective. Together
--- with the up vector, it defines the 3D orientation of the
--- listener in the scene. The direction vector doesn't
--- have to be normalized.
--- The default listener's direction is (0, 0, -1).
---
--- @param direction New listener's direction
---
--- @see `getDirection`, `setUpVector`, `setPosition`
---@type fun(direction: sf.Vector3f)
sf.Listener.setDirection = function() end
--- @brief Get the current forward vector of the listener in the scene
---
--- @return Listener's forward vector (not normalized)
---
--- @see `setDirection`
---@type fun(): sf.Vector3f
sf.Listener.getDirection = function() end
--- @brief Set the velocity of the listener in the scene
---
--- The default listener's velocity is (0, 0, -1).
---
--- @param velocity New listener's velocity
---
--- @see `getVelocity`, `getDirection`, `setUpVector`, `setPosition`
---@type fun(velocity: sf.Vector3f)
sf.Listener.setVelocity = function() end
--- @brief Get the current forward vector of the listener in the scene
---
--- @return Listener's velocity
---
--- @see `setVelocity`
---@type fun(): sf.Vector3f
sf.Listener.getVelocity = function() end
--- @brief Set the cone properties of the listener in the audio scene
---
--- The cone defines how directional attenuation is applied.
--- The default cone of a sound is (2 * PI, 2 * PI, 1).
---
--- @param cone Cone properties of the listener in the scene
---
--- @see `getCone`
---@type fun(cone: sf.Listener.Cone)
sf.Listener.setCone = function() end
--- @brief Get the cone properties of the listener in the audio scene
---
--- @return Cone properties of the listener
---
--- @see `setCone`
---@type fun(): sf.Listener.Cone
sf.Listener.getCone = function() end
--- @brief Set the upward vector of the listener in the scene
---
--- The up vector is the vector that points upward from the
--- listener's perspective. Together with the direction, it
--- defines the 3D orientation of the listener in the scene.
--- The up vector doesn't have to be normalized.
--- The default listener's up vector is (0, 1, 0). It is usually
--- not necessary to change it, especially in 2D scenarios.
---
--- @param upVector New listener's up vector
---
--- @see `getUpVector`, `setDirection`, `setPosition`
---@type fun(upVector: sf.Vector3f)
sf.Listener.setUpVector = function() end
--- @brief Get the current upward vector of the listener in the scene
---
--- @return Listener's upward vector (not normalized)
---
--- @see `setUpVector`
---@type fun(): sf.Vector3f
sf.Listener.getUpVector = function() end
--- @brief Provide write access to sound files
---@class sf.OutputSoundFile
sf.OutputSoundFile = sf.OutputSoundFile or {}
--- @brief Default constructor
---
--- Construct an output sound file that is not associated
--- with a file to write.
---@overload fun(filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): sf.OutputSoundFile
---@return sf.OutputSoundFile
function sf.OutputSoundFile.new() end
--- @brief Open the sound file from the disk for writing
---
--- The supported audio formats are: WAV, OGG/Vorbis, FLAC.
---
--- @param filename     Path of the sound file to write
--- @param sampleRate   Sample rate of the sound
--- @param channelCount Number of channels in the sound
--- @param channelMap   Map of position in sample frame to sound channel
---
--- @return `true` if the file was successfully opened
---@type fun(self: sf.OutputSoundFile, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean
sf.OutputSoundFile.openFromFile = function() end
--- @brief Write audio samples to the file
---
--- @param samples     Pointer to the sample array to write
--- @param count       Number of samples to write
---@type fun(self: sf.OutputSoundFile, samples: any)
sf.OutputSoundFile.write = function() end
--- @brief Close the current file
---@type fun(self: sf.OutputSoundFile)
sf.OutputSoundFile.close = function() end
--- @brief Enumeration of the playback device notifications
---@class sf.PlaybackDevice.Notification
--- Playback device has been started
---@field DeviceStarted sf.PlaybackDevice.Notification
--- Playback device has been stopped
---@field DeviceStopped sf.PlaybackDevice.Notification
--- Playback device has been rerouted (Generated on platforms that support automatic stream routing)
---@field DeviceRerouted sf.PlaybackDevice.Notification
--- Playback device interruption has begun (Generated on Apple mobile platforms)
---@field DeviceInterruptionBegan sf.PlaybackDevice.Notification
--- Playback device interruption has ended (Generated on Apple mobile platforms)
---@field DeviceInterruptionEnded sf.PlaybackDevice.Notification
--- Playback device has been unlocked (Generated by Emscripten/WebAudio)
---@field DeviceUnlocked sf.PlaybackDevice.Notification
sf.PlaybackDevice = sf.PlaybackDevice or {}
sf.PlaybackDevice.Notification = sf.PlaybackDevice.Notification or {}
--- @brief Callable that is called to notify of changes to the playback device state
---@alias sf.PlaybackDevice.NotificationCallback fun(notification: sf.PlaybackDevice.Notification)
--- @brief Get a list of the names of all available audio playback devices
---
--- This function returns a vector of strings containing
--- the names of all available audio playback devices.
---
--- If the operating system reports multiple devices with
--- the same name, a number will be appended to the name
--- of all subsequent devices to distinguish them from each
--- other. This guarantees that every entry returned by this
--- function will represent a unique device.
---
--- For example, if the operating system reports multiple
--- devices with the name "Sound Card", the entries returned
--- would be:
--- - Sound Card
--- - Sound Card 2
--- - Sound Card 3
--- - ...
---
--- The default device, if one is marked as such, will be
--- placed at the beginning of the vector.
---
--- If no devices are available, this function will return
--- an empty vector.
---
--- @return A vector of strings containing the device names or an empty vector if no devices are available
---@type fun(): string[]
sf.PlaybackDevice.getAvailableDevices = function() end
--- @brief Get the name of the default audio playback device
---
--- This function returns the name of the default audio
--- playback device. If none is available, `std::nullopt`
--- is returned.
---
--- Note that depending on when this function is called, the
--- default device reported by the operating system might
--- change e.g. when a USB audio device is plugged into or
--- unplugged from the system.
---
--- @return The name of the default audio playback device
---@type fun(): string|nil
sf.PlaybackDevice.getDefaultDevice = function() end
--- @brief Set the audio playback device
---
--- This function sets the audio playback device to the device
--- with the given `name`. It can be called on the fly (i.e:
--- while sounds are playing).
---
--- If there are sounds playing when the audio playback
--- device is switched, the sounds will continue playing
--- uninterrupted on the new audio playback device.
---
--- @param name The name of the audio playback device
---
--- @return `true`, if it was able to set the requested device
---
--- @see `getAvailableDevices`, `getDefaultDevice`, `setDeviceToDefault`, `setDeviceToNull`
---@type fun(name: string): boolean
sf.PlaybackDevice.setDevice = function() end
--- @brief Set the audio playback device to the default
---
--- This function sets the audio playback device to the
--- default device. It can be called on the fly (i.e:
--- while sounds are playing).
---
--- If there are sounds playing when the audio playback
--- device is switched, the sounds will continue playing
--- uninterrupted on the new audio playback device.
---
--- When certain backends are used, using the default device
--- will enable automatic stream routing. When automatic
--- stream routing is enabled, audio data is automatically
--- sent to whichever physical audio device is currently
--- marked as the default on the system. If the default device
--- changes due to e.g. a device being added to or removed
--- from the system or the user marking another device as the
--- default, automatic stream routing will seamlessly reroute
--- the audio data to the new default device without any
--- manual intervention.
---
--- Automatic stream routing is currently supported when using
--- the WASAPI or DirectSound backend on Windows or the
--- Core Audio backend on macOS and iOS.
---
--- Depending on the order in which hardware devices are
--- initialized e.g. after resuming from sleep, the default
--- device might change one or more times in rapid succession
--- before it reverts back to the state in which it was
--- before the system went to sleep.
---
--- @return `true`, if it was able to set the audio playback device to the default device
---
--- @see `getAvailableDevices`, `getDefaultDevice`, `setDevice`, `setDeviceToNull`
---@type fun(): boolean
sf.PlaybackDevice.setDeviceToDefault = function() end
--- @brief Set the audio playback device to the null device
---
--- This function sets the audio playback device to the
--- null device. It can be called on the fly (i.e:
--- while sounds are playing).
---
--- If there are sounds playing when the audio playback
--- device is switched, the sounds will continue playing
--- uninterrupted on the new audio playback device.
---
--- Audio data routed to the null device will be discarded
--- by the backend. This can be used to keep sounds playing
--- without having them actually output on a physical
--- audio playback device.
---
--- @return `true`, if it was able to set the audio playback device to the null device
---
--- @see `getAvailableDevices`, `getDefaultDevice`, `setDevice`, `setDeviceToDefault`
---@type fun(): boolean
sf.PlaybackDevice.setDeviceToNull = function() end
--- @brief Get the name of the current audio playback device
---
--- @return The name of the current audio playback device or `std::nullopt` if there is none
---@type fun(): string|nil
sf.PlaybackDevice.getDevice = function() end
--- @brief Get the sample rate of the current audio playback device
---
--- @return The sample rate of the current audio playback device or `std::nullopt` if there is none
---@type fun(): integer|nil
sf.PlaybackDevice.getDeviceSampleRate = function() end
--- @brief Check if the current playback device is the default device
---
--- This function will return `false` if there is no
--- current playback device.
---
--- @return `true`, if the current playback device is the default device
---@type fun(): boolean
sf.PlaybackDevice.isDefaultDevice = function() end
--- @brief Set a callback that should be called to notify of changes to the playback device state
---
--- Warning: Do not attempt to alter the device state from
--- within this callback. This includes changing the device
--- and even creating/destroying sound objects since that
--- could indirectly cause the playback device to be
--- created/destroyed. Also do not attempt to set the
--- notification callback from within this callback. Doing
--- so will result in a deadlock.
---
--- When receiving a notification via this callback, store
--- the information somewhere and react on it from another
--- thread e.g. the main thread within the application.
---
--- @param callback The callback that should be called to notify of changes to the playback device state
---@type fun(callback: sf.PlaybackDevice.NotificationCallback|nil)
sf.PlaybackDevice.setNotificationCallback = function() end
--- @ingroup audio
--- @brief Types of sound channels that can be read/written from sound buffers/files
---
--- In multi-channel audio, each sound channel can be
--- assigned a position. The position of the channel is
--- used to determine where to place a sound when it
--- is spatialized. Assigning an incorrect sound channel
--- will result in multi-channel audio being positioned
--- incorrectly when using spatialization.
---@class sf.SoundChannel
---@field Unspecified sf.SoundChannel
---@field Mono sf.SoundChannel
---@field FrontLeft sf.SoundChannel
---@field FrontRight sf.SoundChannel
---@field FrontCenter sf.SoundChannel
---@field FrontLeftOfCenter sf.SoundChannel
---@field FrontRightOfCenter sf.SoundChannel
---@field LowFrequencyEffects sf.SoundChannel
---@field BackLeft sf.SoundChannel
---@field BackRight sf.SoundChannel
---@field BackCenter sf.SoundChannel
---@field SideLeft sf.SoundChannel
---@field SideRight sf.SoundChannel
---@field TopCenter sf.SoundChannel
---@field TopFrontLeft sf.SoundChannel
---@field TopFrontRight sf.SoundChannel
---@field TopFrontCenter sf.SoundChannel
---@field TopBackLeft sf.SoundChannel
---@field TopBackRight sf.SoundChannel
---@field TopBackCenter sf.SoundChannel
sf.SoundChannel = sf.SoundChannel or {}
--- @brief Provide read access to sound files
---@class sf.InputSoundFile
sf.InputSoundFile = sf.InputSoundFile or {}
--- @brief Construct a sound file from the disk for reading
---
--- The supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.
--- The supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.
---
--- @param filename Path of the sound file to load
---
--- @throws sf::Exception if opening the file was unsuccessful
---@overload fun(stream: sf.InputStream): sf.InputSoundFile
---@overload fun(): sf.InputSoundFile
---@overload fun(data: any): sf.InputSoundFile
---@param filename string
---@return sf.InputSoundFile
function sf.InputSoundFile.new(filename) end
--- @brief Open a sound file from the disk for reading
---
--- The supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC, MP3.
--- The supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.
---
--- @param filename Path of the sound file to load
---
--- @return `true` if the file was successfully opened
---@type fun(self: sf.InputSoundFile, filename: string): boolean
sf.InputSoundFile.openFromFile = function() end
--- @brief Open a sound file in memory for reading
---
--- The supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.
--- The supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.
---
--- @param data        Pointer to the file data in memory
--- @param sizeInBytes Size of the data to load, in bytes
---
--- @return `true` if the file was successfully opened
---@type fun(self: sf.InputSoundFile, data: any): boolean
sf.InputSoundFile.openFromMemory = function() end
--- @brief Open a sound file from a custom stream for reading
---
--- The supported audio formats are: WAV (PCM only), OGG/Vorbis, FLAC.
--- The supported sample sizes for FLAC and WAV are 8, 16, 24 and 32 bit.
---
--- @param stream Source stream to read from
---
--- @return `true` if the file was successfully opened
---@type fun(self: sf.InputSoundFile, stream: sf.InputStream): boolean
sf.InputSoundFile.openFromStream = function() end
--- @brief Get the total number of audio samples in the file
---
--- @return Number of samples
---@type fun(self: sf.InputSoundFile): integer
sf.InputSoundFile.getSampleCount = function() end
--- @brief Get the number of channels used by the sound
---
--- @return Number of channels (1 = mono, 2 = stereo)
---@type fun(self: sf.InputSoundFile): integer
sf.InputSoundFile.getChannelCount = function() end
--- @brief Get the sample rate of the sound
---
--- @return Sample rate, in samples per second
---@type fun(self: sf.InputSoundFile): integer
sf.InputSoundFile.getSampleRate = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---
--- @see `getSampleRate`, `getChannelCount`, `getDuration`
---@type fun(self: sf.InputSoundFile): sf.SoundChannel[]
sf.InputSoundFile.getChannelMap = function() end
--- @brief Get the total duration of the sound file
---
--- This function is provided for convenience, the duration is
--- deduced from the other sound file attributes.
---
--- @return Duration of the sound file
---@type fun(self: sf.InputSoundFile): sf.Time
sf.InputSoundFile.getDuration = function() end
--- @brief Get the read offset of the file in time
---
--- @return Time position
---@type fun(self: sf.InputSoundFile): sf.Time
sf.InputSoundFile.getTimeOffset = function() end
--- @brief Get the read offset of the file in samples
---
--- @return Sample position
---@type fun(self: sf.InputSoundFile): integer
sf.InputSoundFile.getSampleOffset = function() end
--- @brief Change the current read position to the given sample offset
---
--- This function takes a sample offset to provide maximum
--- precision. If you need to jump to a given time, use the
--- other overload.
---
--- The sample offset takes the channels into account.
--- If you have a time offset instead, you can easily find
--- the corresponding sample offset with the following formula:
--- `timeInSeconds * sampleRate * channelCount`
--- If the given offset exceeds to total number of samples,
--- this function jumps to the end of the sound file.
---
--- @param sampleOffset Index of the sample to jump to, relative to the beginning
---@overload fun(self: sf.InputSoundFile, timeOffset: sf.Time)
---@param self sf.InputSoundFile
---@param sampleOffset integer
function sf.InputSoundFile.seek(self, sampleOffset) end
--- @brief Read audio samples from the open file
---
--- @param samples  Pointer to the sample array to fill
--- @param maxCount Maximum number of samples to read
---
--- @return Number of samples actually read (may be less than @a maxCount)
---@type fun(self: sf.InputSoundFile, maxCount: integer): integer, any
sf.InputSoundFile.read = function() end
--- @brief Close the current file
---@type fun(self: sf.InputSoundFile)
sf.InputSoundFile.close = function() end
--- @brief Storage for audio samples defining a sound
---@class sf.SoundBuffer
sf.SoundBuffer = sf.SoundBuffer or {}
--- @brief Construct the sound buffer from a file
---
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @param filename Path of the sound file to load
---
--- @throws sf::Exception if loading was unsuccessful
---
--- @see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`
---@overload fun(stream: sf.InputStream): sf.SoundBuffer
---@overload fun(): sf.SoundBuffer
---@overload fun(data: any): sf.SoundBuffer
---@overload fun(samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): sf.SoundBuffer
---@param filename string
---@return sf.SoundBuffer
function sf.SoundBuffer.new(filename) end
--- @brief Load the sound buffer from a file
---
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @param filename Path of the sound file to load
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromMemory`, `loadFromStream`, `loadFromSamples`, `saveToFile`
---@type fun(self: sf.SoundBuffer, filename: string): boolean
sf.SoundBuffer.loadFromFile = function() end
--- @brief Load the sound buffer from a file in memory
---
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @param data        Pointer to the file data in memory
--- @param sizeInBytes Size of the data to load, in bytes
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromFile`, `loadFromStream`, `loadFromSamples`
---@type fun(self: sf.SoundBuffer, data: any): boolean
sf.SoundBuffer.loadFromMemory = function() end
--- @brief Load the sound buffer from a custom stream
---
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @param stream Source stream to read from
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromFile`, `loadFromMemory`, `loadFromSamples`
---@type fun(self: sf.SoundBuffer, stream: sf.InputStream): boolean
sf.SoundBuffer.loadFromStream = function() end
--- @brief Load the sound buffer from an array of audio samples
---
--- The assumed format of the audio samples is 16 bit signed integer.
---
--- @param samples      Pointer to the array of samples in memory
--- @param sampleCount  Number of samples in the array
--- @param channelCount Number of channels (1 = mono, 2 = stereo, ...)
--- @param sampleRate   Sample rate (number of samples to play per second)
--- @param channelMap   Map of position in sample frame to sound channel
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `loadFromFile`, `loadFromMemory`, `saveToFile`
---@type fun(self: sf.SoundBuffer, samples: any, channelCount: integer, sampleRate: integer, channelMap: sf.SoundChannel[]): boolean
sf.SoundBuffer.loadFromSamples = function() end
--- @brief Save the sound buffer to an audio file
---
--- See the documentation of `sf::OutputSoundFile` for the list
--- of supported formats.
---
--- @param filename Path of the sound file to write
---
--- @return `true` if saving succeeded, `false` if it failed
---@type fun(self: sf.SoundBuffer, filename: string): boolean
sf.SoundBuffer.saveToFile = function() end
--- @brief Get the array of audio samples stored in the buffer
---
--- The format of the returned samples is 16 bit signed integer.
--- The total number of samples in this array is given by the
--- `getSampleCount()` function.
---
--- @return Read-only pointer to the array of sound samples
---
--- @see `getSampleCount`
---@type fun(self: sf.SoundBuffer): integer[]
sf.SoundBuffer.getSamples = function() end
--- @brief Get the number of samples stored in the buffer
---
--- The array of samples can be accessed with the `getSamples()`
--- function.
---
--- @return Number of samples
---
--- @see `getSamples`
---@type fun(self: sf.SoundBuffer): integer
sf.SoundBuffer.getSampleCount = function() end
--- @brief Get the sample rate of the sound
---
--- The sample rate is the number of samples played per second.
--- The higher, the better the quality (for example, 44100
--- samples/s is CD quality).
---
--- @return Sample rate (number of samples per second)
---
--- @see `getChannelCount`, `getChannelMap`, `getDuration`
---@type fun(self: sf.SoundBuffer): integer
sf.SoundBuffer.getSampleRate = function() end
--- @brief Get the number of channels used by the sound
---
--- If the sound is mono then the number of channels will
--- be 1, 2 for stereo, etc.
---
--- @return Number of channels
---
--- @see `getSampleRate`, `getChannelMap`, `getDuration`
---@type fun(self: sf.SoundBuffer): integer
sf.SoundBuffer.getChannelCount = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---
--- @see `getSampleRate`, `getChannelCount`, `getDuration`
---@type fun(self: sf.SoundBuffer): sf.SoundChannel[]
sf.SoundBuffer.getChannelMap = function() end
--- @brief Get the total duration of the sound
---
--- @return Sound duration
---
--- @see `getSampleRate`, `getChannelCount`, `getChannelMap`
---@type fun(self: sf.SoundBuffer): sf.Time
sf.SoundBuffer.getDuration = function() end
--- @brief Abstract base class for sound file decoding
---@class sf.SoundFileReader
sf.SoundFileReader = sf.SoundFileReader or {}
--- @brief Open a sound file for reading
---
--- The provided stream reference is valid as long as the
--- `SoundFileReader` is alive, so it is safe to use/store it
--- during the whole lifetime of the reader.
---
--- @param stream Source stream to read from
---
--- @return Properties of the loaded sound if the file was successfully opened, `std::nullopt` otherwise
---@type fun(self: sf.SoundFileReader, stream: sf.InputStream): sf.SoundFileReader.Info|nil
sf.SoundFileReader.open = function() end
--- @brief Change the current read position to the given sample offset
---
--- The sample offset takes the channels into account.
--- If you have a time offset instead, you can easily find
--- the corresponding sample offset with the following formula:
--- `timeInSeconds * sampleRate * channelCount`
--- If the given offset exceeds to total number of samples,
--- this function must jump to the end of the file.
---
--- @param sampleOffset Index of the sample to jump to, relative to the beginning
---@type fun(self: sf.SoundFileReader, sampleOffset: integer)
sf.SoundFileReader.seek = function() end
--- @brief Read audio samples from the open file
---
--- @param samples  Pointer to the sample array to fill
--- @param maxCount Maximum number of samples to read
---
--- @return Number of samples actually read (may be less than @a maxCount)
---@type fun(self: sf.SoundFileReader, maxCount: integer): integer, any
sf.SoundFileReader.read = function() end
--- @brief Structure holding the audio properties of a sound file
---@class sf.SoundFileReader.Info
--- Total number of samples in the file
---@field sampleCount integer
--- Number of channels of the sound
---@field channelCount integer
--- Samples rate of the sound, in samples per second
---@field sampleRate integer
--- Map of position in sample frame to sound channel
---@field channelMap sf.SoundChannel[]
sf.SoundFileReader.Info = sf.SoundFileReader.Info or {}
---@type fun(): sf.SoundFileReader.Info
sf.SoundFileReader.Info.new = function() end
--- @brief Abstract base class for sound file encoding
---@class sf.SoundFileWriter
sf.SoundFileWriter = sf.SoundFileWriter or {}
--- @brief Open a sound file for writing
---
--- @param filename     Path of the file to open
--- @param sampleRate   Sample rate of the sound
--- @param channelCount Number of channels of the sound
--- @param channelMap   Map of position in sample frame to sound channel
---
--- @return `true` if the file was successfully opened
---@type fun(self: sf.SoundFileWriter, filename: string, sampleRate: integer, channelCount: integer, channelMap: sf.SoundChannel[]): boolean
sf.SoundFileWriter.open = function() end
--- @brief Write audio samples to the open file
---
--- @param samples Pointer to the sample array to write
--- @param count   Number of samples to write
---@type fun(self: sf.SoundFileWriter, samples: any)
sf.SoundFileWriter.write = function() end
--- @brief Abstract base class for capturing sound data
---@class sf.SoundRecorder
sf.SoundRecorder = sf.SoundRecorder or {}
--- @brief Start the capture
---
--- The `sampleRate` parameter defines the number of audio samples
--- captured per second. The higher, the better the quality
--- (for example, 44100 samples/sec is CD quality).
--- This function uses its own thread so that it doesn't block
--- the rest of the program while the capture runs.
--- Please note that only one capture can happen at the same time.
--- You can select which capture device will be used by passing
--- the name to the `setDevice()` method. If none was selected
--- before, the default capture device will be used. You can get a
--- list of the names of all available capture devices by calling
--- `getAvailableDevices()`.
---
--- @param sampleRate Desired capture rate, in number of samples per second
---
--- @return `true`, if start of capture was successful
---
--- @see `stop`, `getAvailableDevices`
---@overload fun(self: sf.SoundRecorder): boolean
---@param self sf.SoundRecorder
---@param sampleRate integer
---@return boolean
function sf.SoundRecorder.start(self, sampleRate) end
--- @brief Stop the capture
---
--- @see `start`
---@type fun(self: sf.SoundRecorder)
sf.SoundRecorder.stop = function() end
--- @brief Get the sample rate
---
--- The sample rate defines the number of audio samples
--- captured per second. The higher, the better the quality
--- (for example, 44100 samples/sec is CD quality).
---
--- @return Sample rate, in samples per second
---@type fun(self: sf.SoundRecorder): integer
sf.SoundRecorder.getSampleRate = function() end
--- @brief Get a list of the names of all available audio capture devices
---
--- This function returns a vector of strings, containing
--- the names of all available audio capture devices.
---
--- @return A vector of strings containing the names
---@type fun(): string[]
sf.SoundRecorder.getAvailableDevices = function() end
--- @brief Get the name of the default audio capture device
---
--- This function returns the name of the default audio
--- capture device. If none is available, an empty string
--- is returned.
---
--- @return The name of the default audio capture device
---@type fun(): string
sf.SoundRecorder.getDefaultDevice = function() end
--- @brief Set the audio capture device
---
--- This function sets the audio capture device to the device
--- with the given `name`. It can be called on the fly (i.e:
--- while recording). If you do so while recording and
--- opening the device fails, it stops the recording.
---
--- @param name The name of the audio capture device
---
--- @return `true`, if it was able to set the requested device
---
--- @see `getAvailableDevices`, `getDefaultDevice`
---@type fun(self: sf.SoundRecorder, name: string): boolean
sf.SoundRecorder.setDevice = function() end
--- @brief Get the name of the current audio capture device
---
--- @return The name of the current audio capture device
---@type fun(self: sf.SoundRecorder): string
sf.SoundRecorder.getDevice = function() end
--- @brief Set the channel count of the audio capture device
---
--- This method allows you to specify the number of channels
--- used for recording. Currently only 16-bit mono and
--- 16-bit stereo are supported.
---
--- @param channelCount Number of channels. Currently only
--- mono (1) and stereo (2) are supported.
---
--- @see `getChannelCount`
---@type fun(self: sf.SoundRecorder, channelCount: integer)
sf.SoundRecorder.setChannelCount = function() end
--- @brief Get the number of channels used by this recorder
---
--- Currently only mono and stereo are supported, so the
--- value is either 1 (for mono) or 2 (for stereo).
---
--- @return Number of channels
---
--- @see `setChannelCount`
---@type fun(self: sf.SoundRecorder): integer
sf.SoundRecorder.getChannelCount = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---@type fun(self: sf.SoundRecorder): sf.SoundChannel[]
sf.SoundRecorder.getChannelMap = function() end
--- @brief Check if the system supports audio capture
---
--- This function should always be called before using
--- the audio capture features. If it returns `false`, then
--- any attempt to use `sf::SoundRecorder` or one of its derived
--- classes will fail.
---
--- @return `true` if audio capture is supported, `false` otherwise
---@type fun(): boolean
sf.SoundRecorder.isAvailable = function() end
--- @brief Specialized SoundRecorder which stores the captured
--- audio data into a sound buffer
---@class sf.SoundBufferRecorder : sf.SoundRecorder
sf.SoundBufferRecorder = sf.SoundBufferRecorder or {}
---@type fun(): sf.SoundBufferRecorder
sf.SoundBufferRecorder.new = function() end
--- @brief Start the capture
---
--- The `sampleRate` parameter defines the number of audio samples
--- captured per second. The higher, the better the quality
--- (for example, 44100 samples/sec is CD quality).
--- This function uses its own thread so that it doesn't block
--- the rest of the program while the capture runs.
--- Please note that only one capture can happen at the same time.
--- You can select which capture device will be used by passing
--- the name to the `setDevice()` method. If none was selected
--- before, the default capture device will be used. You can get a
--- list of the names of all available capture devices by calling
--- `getAvailableDevices()`.
---
--- @param sampleRate Desired capture rate, in number of samples per second
---
--- @return `true`, if start of capture was successful
---
--- @see `stop`, `getAvailableDevices`
---@overload fun(self: sf.SoundBufferRecorder): boolean
---@param self sf.SoundBufferRecorder
---@param sampleRate integer
---@return boolean
function sf.SoundBufferRecorder.start(self, sampleRate) end
--- @brief Stop the capture
---
--- @see `start`
---@type fun(self: sf.SoundBufferRecorder)
sf.SoundBufferRecorder.stop = function() end
--- @brief Get the sample rate
---
--- The sample rate defines the number of audio samples
--- captured per second. The higher, the better the quality
--- (for example, 44100 samples/sec is CD quality).
---
--- @return Sample rate, in samples per second
---@type fun(self: sf.SoundBufferRecorder): integer
sf.SoundBufferRecorder.getSampleRate = function() end
--- @brief Get a list of the names of all available audio capture devices
---
--- This function returns a vector of strings, containing
--- the names of all available audio capture devices.
---
--- @return A vector of strings containing the names
---@type fun(): string[]
sf.SoundBufferRecorder.getAvailableDevices = function() end
--- @brief Get the name of the default audio capture device
---
--- This function returns the name of the default audio
--- capture device. If none is available, an empty string
--- is returned.
---
--- @return The name of the default audio capture device
---@type fun(): string
sf.SoundBufferRecorder.getDefaultDevice = function() end
--- @brief Set the audio capture device
---
--- This function sets the audio capture device to the device
--- with the given `name`. It can be called on the fly (i.e:
--- while recording). If you do so while recording and
--- opening the device fails, it stops the recording.
---
--- @param name The name of the audio capture device
---
--- @return `true`, if it was able to set the requested device
---
--- @see `getAvailableDevices`, `getDefaultDevice`
---@type fun(self: sf.SoundBufferRecorder, name: string): boolean
sf.SoundBufferRecorder.setDevice = function() end
--- @brief Get the name of the current audio capture device
---
--- @return The name of the current audio capture device
---@type fun(self: sf.SoundBufferRecorder): string
sf.SoundBufferRecorder.getDevice = function() end
--- @brief Set the channel count of the audio capture device
---
--- This method allows you to specify the number of channels
--- used for recording. Currently only 16-bit mono and
--- 16-bit stereo are supported.
---
--- @param channelCount Number of channels. Currently only
--- mono (1) and stereo (2) are supported.
---
--- @see `getChannelCount`
---@type fun(self: sf.SoundBufferRecorder, channelCount: integer)
sf.SoundBufferRecorder.setChannelCount = function() end
--- @brief Get the number of channels used by this recorder
---
--- Currently only mono and stereo are supported, so the
--- value is either 1 (for mono) or 2 (for stereo).
---
--- @return Number of channels
---
--- @see `setChannelCount`
---@type fun(self: sf.SoundBufferRecorder): integer
sf.SoundBufferRecorder.getChannelCount = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---@type fun(self: sf.SoundBufferRecorder): sf.SoundChannel[]
sf.SoundBufferRecorder.getChannelMap = function() end
--- @brief Check if the system supports audio capture
---
--- This function should always be called before using
--- the audio capture features. If it returns `false`, then
--- any attempt to use `sf::SoundRecorder` or one of its derived
--- classes will fail.
---
--- @return `true` if audio capture is supported, `false` otherwise
---@type fun(): boolean
sf.SoundBufferRecorder.isAvailable = function() end
--- @brief Get the sound buffer containing the captured audio data
---
--- The sound buffer is valid only after the capture has ended.
--- This function provides a read-only access to the internal
--- sound buffer, but it can be copied if you need to
--- make any modification to it.
---
--- @return Read-only access to the sound buffer
---@type fun(self: sf.SoundBufferRecorder): sf.SoundBuffer
sf.SoundBufferRecorder.getBuffer = function() end
--- @brief Base class defining a sound's properties
---@class sf.SoundSource
sf.SoundSource = sf.SoundSource or {}
--- @brief Set the pitch of the sound
---
--- The pitch represents the perceived fundamental frequency
--- of a sound; thus you can make a sound more acute or grave
--- by changing its pitch. A side effect of changing the pitch
--- is to modify the playing speed of the sound as well.
--- The default value for the pitch is 1.
---
--- @param pitch New pitch to apply to the sound
---
--- @see `getPitch`
---@type fun(self: sf.SoundSource, pitch: number)
sf.SoundSource.setPitch = function() end
--- @brief Set the pan of the sound
---
--- Using panning, a mono sound can be panned between
--- stereo channels. When the pan is set to -1, the sound
--- is played only on the left channel, when the pan is set
--- to +1, the sound is played only on the right channel.
---
--- @param pan New pan to apply to the sound [-1, +1]
---
--- @see `getPan`
---@type fun(self: sf.SoundSource, pan: number)
sf.SoundSource.setPan = function() end
--- @brief Set the volume of the sound
---
--- The volume is a value between 0 (mute) and 100 (full volume).
--- The default value for the volume is 100.
---
--- @param volume Volume of the sound
---
--- @see `getVolume`
---@type fun(self: sf.SoundSource, volume: number)
sf.SoundSource.setVolume = function() end
--- @brief Set whether spatialization of the sound is enabled
---
--- Spatialization is the application of various effects to
--- simulate a sound being emitted at a virtual position in
--- 3D space and exhibiting various physical phenomena such as
--- directional attenuation and doppler shift.
---
--- @param enabled `true` to enable spatialization, `false` to disable
---
--- @see `isSpatializationEnabled`
---@type fun(self: sf.SoundSource, enabled: boolean)
sf.SoundSource.setSpatializationEnabled = function() end
--- @brief Set the 3D position of the sound in the audio scene
---
--- Only sounds with one channel (mono sounds) can be
--- spatialized.
--- The default position of a sound is (0, 0, 0).
---
--- @param position Position of the sound in the scene
---
--- @see `getPosition`
---@type fun(self: sf.SoundSource, position: sf.Vector3f)
sf.SoundSource.setPosition = function() end
--- @brief Set the 3D direction of the sound in the audio scene
---
--- The direction defines where the sound source is facing
--- in 3D space. It will affect how the sound is attenuated
--- if facing away from the listener.
--- The default direction of a sound is (0, 0, -1).
---
--- @param direction Direction of the sound in the scene
---
--- @see `getDirection`
---@type fun(self: sf.SoundSource, direction: sf.Vector3f)
sf.SoundSource.setDirection = function() end
--- @brief Set the cone properties of the sound in the audio scene
---
--- The cone defines how directional attenuation is applied.
--- The default cone of a sound is (2 * PI, 2 * PI, 1).
---
--- @param cone Cone properties of the sound in the scene
---
--- @see `getCone`
---@type fun(self: sf.SoundSource, cone: sf.SoundSource.Cone)
sf.SoundSource.setCone = function() end
--- @brief Set the 3D velocity of the sound in the audio scene
---
--- The velocity is used to determine how to doppler shift
--- the sound. Sounds moving towards the listener will be
--- perceived to have a higher pitch and sounds moving away
--- from the listener will be perceived to have a lower pitch.
---
--- @param velocity Velocity of the sound in the scene
---
--- @see `getVelocity`
---@type fun(self: sf.SoundSource, velocity: sf.Vector3f)
sf.SoundSource.setVelocity = function() end
--- @brief Set the doppler factor of the sound
---
--- The doppler factor determines how strong the doppler
--- shift will be.
---
--- @param factor New doppler factor to apply to the sound
---
--- @see `getDopplerFactor`
---@type fun(self: sf.SoundSource, factor: number)
sf.SoundSource.setDopplerFactor = function() end
--- @brief Set the directional attenuation factor of the sound
---
--- Depending on the virtual position of an output channel
--- relative to the listener (such as in surround sound
--- setups), sounds will be attenuated when emitting them
--- from certain channels. This factor determines how strong
--- the attenuation based on output channel position
--- relative to the listener is.
---
--- @param factor New directional attenuation factor to apply to the sound
---
--- @see `getDirectionalAttenuationFactor`
---@type fun(self: sf.SoundSource, factor: number)
sf.SoundSource.setDirectionalAttenuationFactor = function() end
--- @brief Make the sound's position relative to the listener or absolute
---
--- Making a sound relative to the listener will ensure that it will always
--- be played the same way regardless of the position of the listener.
--- This can be useful for non-spatialized sounds, sounds that are
--- produced by the listener, or sounds attached to it.
--- The default value is `false` (position is absolute).
---
--- @param relative `true` to set the position relative, `false` to set it absolute
---
--- @see `isRelativeToListener`
---@type fun(self: sf.SoundSource, relative: boolean)
sf.SoundSource.setRelativeToListener = function() end
--- @brief Set the minimum distance of the sound
---
--- The "minimum distance" of a sound is the maximum
--- distance at which it is heard at its maximum volume. Further
--- than the minimum distance, it will start to fade out according
--- to its attenuation factor. A value of 0 ("inside the head
--- of the listener") is an invalid value and is forbidden.
--- The default value of the minimum distance is 1.
---
--- @param distance New minimum distance of the sound
---
--- @see `getMinDistance`, `setAttenuation`
---@type fun(self: sf.SoundSource, distance: number)
sf.SoundSource.setMinDistance = function() end
--- @brief Set the maximum distance of the sound
---
--- The "maximum distance" of a sound is the minimum
--- distance at which it is heard at its minimum volume. Closer
--- than the maximum distance, it will start to fade in according
--- to its attenuation factor.
--- The default value of the maximum distance is the maximum
--- value a float can represent.
---
--- @param distance New maximum distance of the sound
---
--- @see `getMaxDistance`, `setAttenuation`
---@type fun(self: sf.SoundSource, distance: number)
sf.SoundSource.setMaxDistance = function() end
--- @brief Set the minimum gain of the sound
---
--- When the sound is further away from the listener than
--- the "maximum distance" the attenuated gain is clamped
--- so it cannot go below the minimum gain value.
---
--- @param gain New minimum gain of the sound
---
--- @see `getMinGain`, `setAttenuation`
---@type fun(self: sf.SoundSource, gain: number)
sf.SoundSource.setMinGain = function() end
--- @brief Set the maximum gain of the sound
---
--- When the sound is closer from the listener than
--- the "minimum distance" the attenuated gain is clamped
--- so it cannot go above the maximum gain value.
---
--- @param gain New maximum gain of the sound
---
--- @see `getMaxGain`, `setAttenuation`
---@type fun(self: sf.SoundSource, gain: number)
sf.SoundSource.setMaxGain = function() end
--- @brief Set the attenuation factor of the sound
---
--- The attenuation is a multiplicative factor which makes
--- the sound more or less loud according to its distance
--- from the listener. An attenuation of 0 will produce a
--- non-attenuated sound, i.e. its volume will always be the same
--- whether it is heard from near or from far. On the other hand,
--- an attenuation value such as 100 will make the sound fade out
--- very quickly as it gets further from the listener.
--- The default value of the attenuation is 1.
---
--- @param attenuation New attenuation factor of the sound
---
--- @see `getAttenuation`, `setMinDistance`
---@type fun(self: sf.SoundSource, attenuation: number)
sf.SoundSource.setAttenuation = function() end
--- @brief Set the effect processor to be applied to the sound
---
--- The effect processor is a callable that will be called
--- with sound data to be processed.
---
--- @param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing
---@type fun(self: sf.SoundSource, effectProcessor: sf.SoundSource.EffectProcessor|nil)
sf.SoundSource.setEffectProcessor = function() end
--- @brief Get the pitch of the sound
---
--- @return Pitch of the sound
---
--- @see `setPitch`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getPitch = function() end
--- @brief Get the pan of the sound
---
--- @return Pan of the sound
---
--- @see `setPan`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getPan = function() end
--- @brief Get the volume of the sound
---
--- @return Volume of the sound, in the range [0, 100]
---
--- @see `setVolume`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getVolume = function() end
--- @brief Tell whether spatialization of the sound is enabled
---
--- @return `true` if spatialization is enabled, `false` if it's disabled
---
--- @see `setSpatializationEnabled`
---@type fun(self: sf.SoundSource): boolean
sf.SoundSource.isSpatializationEnabled = function() end
--- @brief Get the 3D position of the sound in the audio scene
---
--- @return Position of the sound
---
--- @see `setPosition`
---@type fun(self: sf.SoundSource): sf.Vector3f
sf.SoundSource.getPosition = function() end
--- @brief Get the 3D direction of the sound in the audio scene
---
--- @return Direction of the sound
---
--- @see `setDirection`
---@type fun(self: sf.SoundSource): sf.Vector3f
sf.SoundSource.getDirection = function() end
--- @brief Get the cone properties of the sound in the audio scene
---
--- @return Cone properties of the sound
---
--- @see `setCone`
---@type fun(self: sf.SoundSource): sf.SoundSource.Cone
sf.SoundSource.getCone = function() end
--- @brief Get the 3D velocity of the sound in the audio scene
---
--- @return Velocity of the sound
---
--- @see `setVelocity`
---@type fun(self: sf.SoundSource): sf.Vector3f
sf.SoundSource.getVelocity = function() end
--- @brief Get the doppler factor of the sound
---
--- @return Doppler factor of the sound
---
--- @see `setDopplerFactor`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getDopplerFactor = function() end
--- @brief Get the directional attenuation factor of the sound
---
--- @return Directional attenuation factor of the sound
---
--- @see `setDirectionalAttenuationFactor`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getDirectionalAttenuationFactor = function() end
--- @brief Tell whether the sound's position is relative to the
--- listener or is absolute
---
--- @return `true` if the position is relative, `false` if it's absolute
---
--- @see `setRelativeToListener`
---@type fun(self: sf.SoundSource): boolean
sf.SoundSource.isRelativeToListener = function() end
--- @brief Get the minimum distance of the sound
---
--- @return Minimum distance of the sound
---
--- @see `setMinDistance`, `getAttenuation`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getMinDistance = function() end
--- @brief Get the maximum distance of the sound
---
--- @return Maximum distance of the sound
---
--- @see `setMaxDistance`, `getAttenuation`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getMaxDistance = function() end
--- @brief Get the minimum gain of the sound
---
--- @return Minimum gain of the sound
---
--- @see `setMinGain`, `getAttenuation`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getMinGain = function() end
--- @brief Get the maximum gain of the sound
---
--- @return Maximum gain of the sound
---
--- @see `setMaxGain`, `getAttenuation`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getMaxGain = function() end
--- @brief Get the attenuation factor of the sound
---
--- @return Attenuation factor of the sound
---
--- @see `setAttenuation`, `getMinDistance`
---@type fun(self: sf.SoundSource): number
sf.SoundSource.getAttenuation = function() end
--- @brief Start or resume playing the sound source
---
--- This function starts the source if it was stopped, resumes
--- it if it was paused, and restarts it from the beginning if
--- it was already playing.
---
--- @see `pause`, `stop`
---@type fun(self: sf.SoundSource)
sf.SoundSource.play = function() end
--- @brief Pause the sound source
---
--- This function pauses the source if it was playing,
--- otherwise (source already paused or stopped) it has no effect.
---
--- @see `play`, `stop`
---@type fun(self: sf.SoundSource)
sf.SoundSource.pause = function() end
--- @brief Stop playing the sound source
---
--- This function stops the source if it was playing or paused,
--- and does nothing if it was already stopped.
--- It also resets the playing position (unlike `pause()`).
---
--- @see `play`, `pause`
---@type fun(self: sf.SoundSource)
sf.SoundSource.stop = function() end
--- @brief Get the current status of the sound (stopped, paused, playing)
---
--- @return Current status of the sound
---@type fun(self: sf.SoundSource): sf.SoundSource.Status
sf.SoundSource.getStatus = function() end
--- @brief Enumeration of the sound source states
---@class sf.SoundSource.Status
--- Sound is not playing
---@field Stopped sf.SoundSource.Status
--- Sound is paused
---@field Paused sf.SoundSource.Status
--- Sound is playing
---@field Playing sf.SoundSource.Status
sf.SoundSource.Status = sf.SoundSource.Status or {}
--- @brief Structure defining the properties of a directional cone
---
--- Sounds will play at gain 1 when the listener
--- is positioned within the inner angle of the cone.
--- Sounds will play at `outerGain` when the listener is
--- positioned outside the outer angle of the cone.
--- The gain declines linearly from 1 to `outerGain` as the
--- listener moves from the inner angle to the outer angle.
---@class sf.SoundSource.Cone
--- Inner angle
---@field innerAngle sf.Angle
--- Outer angle
---@field outerAngle sf.Angle
--- Outer gain
---@field outerGain number
sf.SoundSource.Cone = sf.SoundSource.Cone or {}
---@type fun(): sf.SoundSource.Cone
sf.SoundSource.Cone.new = function() end
--- @brief Callable that is provided with sound data for processing
---
--- When the audio engine sources sound data from sound
--- sources it will pass the data through an effects
--- processor if one is set. The sound data will already be
--- converted to the internal floating point format and have
--- the same sample rate as the audio device and engine. The
--- device sample rate can differ from the sample rate of
--- the source data so keep this in mind when setting up
--- processing that is dependent on the sample rate. The
--- sample rate of the current playback device can be
--- retrieved using `sf::PlaybackDevice::getDeviceSampleRate()`.
---
--- Sound data that is processed this way is provided in
--- frames. Each frame contains 1 floating point sample per
--- channel. If e.g. the data source provides stereo data,
--- each frame will contain 2 floats.
---
--- The effects processor function takes 4 parameters:
--- - The input data frames, channels interleaved
--- - The number of input data frames available
--- - The buffer to write output data frames to, channels interleaved
--- - The number of output data frames that the output buffer can hold
--- - The channel count
---
--- The input and output frame counts are in/out parameters.
---
--- When this function is called, the input count will
--- contain the number of frames available in the input
--- buffer. The output count will contain the size of the
--- output buffer i.e. the maximum number of frames that
--- can be written to the output buffer.
---
--- Attempting to read more frames than the input frame
--- count or write more frames than the output frame count
--- will result in undefined behaviour.
---
--- It is important to note that the channel count of the
--- audio engine currently sourcing data from this sound
--- will always be provided in `frameChannelCount`. This can
--- be different from the channel count of the sound source
--- so make sure to size necessary processing buffers
--- according to the engine channel count and not the sound
--- source channel count.
---
--- When done processing the frames, the input and output
--- frame counts must be updated to reflect the actual
--- number of frames that were read from the input and
--- written to the output.
---
--- The processing function should always try to process as
--- much sound data as possible i.e. always try to fill the
--- output buffer to the maximum. In certain situations for
--- specific effects it can be possible that the input frame
--- count and output frame count aren't equal. As long as
--- the frame counts are updated accordingly this is
--- perfectly valid.
---
--- If the audio engine determines that no audio data is
--- available from the data source, the input data frames
--- pointer is set to `nullptr` and the input frame count is
--- set to 0. In this case it is up to the function to
--- decide how to handle the situation. For specific effects
--- e.g. Echo/Delay buffered data might still be able to be
--- written to the output buffer even if there is no longer
--- any input data.
---
--- An important thing to remember is that this function is
--- directly called by the audio engine. Because the audio
--- engine runs on an internal thread of its own, make sure
--- access to shared data is synchronized appropriately.
---
--- Because this function is stored by the `SoundSource`
--- object it will be able to be called as long as the
--- `SoundSource` object hasn't yet been destroyed. Make sure
--- that any data this function references outlives the
--- SoundSource object otherwise use-after-free errors will
--- occur.
---@alias sf.SoundSource.EffectProcessor fun(inputFrames: number[]|nil, inputFrameCount: integer, outputFrames: number[], outputFrameCount: integer, frameChannelCount: integer): {inputFrameCount: integer, outputFrameCount: integer, outputFrames: number[]?}
--- @brief Regular sound that can be played in the audio environment
---@class sf.Sound : sf.SoundSource
sf.Sound = sf.Sound or {}
--- @brief Construct the sound with a buffer
---
--- @param buffer Sound buffer containing the audio data to play with the sound
---@type fun(buffer: sf.SoundBuffer): sf.Sound
sf.Sound.new = function() end
--- @brief Set the pitch of the sound
---
--- The pitch represents the perceived fundamental frequency
--- of a sound; thus you can make a sound more acute or grave
--- by changing its pitch. A side effect of changing the pitch
--- is to modify the playing speed of the sound as well.
--- The default value for the pitch is 1.
---
--- @param pitch New pitch to apply to the sound
---
--- @see `getPitch`
---@type fun(self: sf.Sound, pitch: number)
sf.Sound.setPitch = function() end
--- @brief Set the pan of the sound
---
--- Using panning, a mono sound can be panned between
--- stereo channels. When the pan is set to -1, the sound
--- is played only on the left channel, when the pan is set
--- to +1, the sound is played only on the right channel.
---
--- @param pan New pan to apply to the sound [-1, +1]
---
--- @see `getPan`
---@type fun(self: sf.Sound, pan: number)
sf.Sound.setPan = function() end
--- @brief Set the volume of the sound
---
--- The volume is a value between 0 (mute) and 100 (full volume).
--- The default value for the volume is 100.
---
--- @param volume Volume of the sound
---
--- @see `getVolume`
---@type fun(self: sf.Sound, volume: number)
sf.Sound.setVolume = function() end
--- @brief Set whether spatialization of the sound is enabled
---
--- Spatialization is the application of various effects to
--- simulate a sound being emitted at a virtual position in
--- 3D space and exhibiting various physical phenomena such as
--- directional attenuation and doppler shift.
---
--- @param enabled `true` to enable spatialization, `false` to disable
---
--- @see `isSpatializationEnabled`
---@type fun(self: sf.Sound, enabled: boolean)
sf.Sound.setSpatializationEnabled = function() end
--- @brief Set the 3D position of the sound in the audio scene
---
--- Only sounds with one channel (mono sounds) can be
--- spatialized.
--- The default position of a sound is (0, 0, 0).
---
--- @param position Position of the sound in the scene
---
--- @see `getPosition`
---@type fun(self: sf.Sound, position: sf.Vector3f)
sf.Sound.setPosition = function() end
--- @brief Set the 3D direction of the sound in the audio scene
---
--- The direction defines where the sound source is facing
--- in 3D space. It will affect how the sound is attenuated
--- if facing away from the listener.
--- The default direction of a sound is (0, 0, -1).
---
--- @param direction Direction of the sound in the scene
---
--- @see `getDirection`
---@type fun(self: sf.Sound, direction: sf.Vector3f)
sf.Sound.setDirection = function() end
--- @brief Set the cone properties of the sound in the audio scene
---
--- The cone defines how directional attenuation is applied.
--- The default cone of a sound is (2 * PI, 2 * PI, 1).
---
--- @param cone Cone properties of the sound in the scene
---
--- @see `getCone`
---@type fun(self: sf.Sound, cone: sf.SoundSource.Cone)
sf.Sound.setCone = function() end
--- @brief Set the 3D velocity of the sound in the audio scene
---
--- The velocity is used to determine how to doppler shift
--- the sound. Sounds moving towards the listener will be
--- perceived to have a higher pitch and sounds moving away
--- from the listener will be perceived to have a lower pitch.
---
--- @param velocity Velocity of the sound in the scene
---
--- @see `getVelocity`
---@type fun(self: sf.Sound, velocity: sf.Vector3f)
sf.Sound.setVelocity = function() end
--- @brief Set the doppler factor of the sound
---
--- The doppler factor determines how strong the doppler
--- shift will be.
---
--- @param factor New doppler factor to apply to the sound
---
--- @see `getDopplerFactor`
---@type fun(self: sf.Sound, factor: number)
sf.Sound.setDopplerFactor = function() end
--- @brief Set the directional attenuation factor of the sound
---
--- Depending on the virtual position of an output channel
--- relative to the listener (such as in surround sound
--- setups), sounds will be attenuated when emitting them
--- from certain channels. This factor determines how strong
--- the attenuation based on output channel position
--- relative to the listener is.
---
--- @param factor New directional attenuation factor to apply to the sound
---
--- @see `getDirectionalAttenuationFactor`
---@type fun(self: sf.Sound, factor: number)
sf.Sound.setDirectionalAttenuationFactor = function() end
--- @brief Make the sound's position relative to the listener or absolute
---
--- Making a sound relative to the listener will ensure that it will always
--- be played the same way regardless of the position of the listener.
--- This can be useful for non-spatialized sounds, sounds that are
--- produced by the listener, or sounds attached to it.
--- The default value is `false` (position is absolute).
---
--- @param relative `true` to set the position relative, `false` to set it absolute
---
--- @see `isRelativeToListener`
---@type fun(self: sf.Sound, relative: boolean)
sf.Sound.setRelativeToListener = function() end
--- @brief Set the minimum distance of the sound
---
--- The "minimum distance" of a sound is the maximum
--- distance at which it is heard at its maximum volume. Further
--- than the minimum distance, it will start to fade out according
--- to its attenuation factor. A value of 0 ("inside the head
--- of the listener") is an invalid value and is forbidden.
--- The default value of the minimum distance is 1.
---
--- @param distance New minimum distance of the sound
---
--- @see `getMinDistance`, `setAttenuation`
---@type fun(self: sf.Sound, distance: number)
sf.Sound.setMinDistance = function() end
--- @brief Set the maximum distance of the sound
---
--- The "maximum distance" of a sound is the minimum
--- distance at which it is heard at its minimum volume. Closer
--- than the maximum distance, it will start to fade in according
--- to its attenuation factor.
--- The default value of the maximum distance is the maximum
--- value a float can represent.
---
--- @param distance New maximum distance of the sound
---
--- @see `getMaxDistance`, `setAttenuation`
---@type fun(self: sf.Sound, distance: number)
sf.Sound.setMaxDistance = function() end
--- @brief Set the minimum gain of the sound
---
--- When the sound is further away from the listener than
--- the "maximum distance" the attenuated gain is clamped
--- so it cannot go below the minimum gain value.
---
--- @param gain New minimum gain of the sound
---
--- @see `getMinGain`, `setAttenuation`
---@type fun(self: sf.Sound, gain: number)
sf.Sound.setMinGain = function() end
--- @brief Set the maximum gain of the sound
---
--- When the sound is closer from the listener than
--- the "minimum distance" the attenuated gain is clamped
--- so it cannot go above the maximum gain value.
---
--- @param gain New maximum gain of the sound
---
--- @see `getMaxGain`, `setAttenuation`
---@type fun(self: sf.Sound, gain: number)
sf.Sound.setMaxGain = function() end
--- @brief Set the attenuation factor of the sound
---
--- The attenuation is a multiplicative factor which makes
--- the sound more or less loud according to its distance
--- from the listener. An attenuation of 0 will produce a
--- non-attenuated sound, i.e. its volume will always be the same
--- whether it is heard from near or from far. On the other hand,
--- an attenuation value such as 100 will make the sound fade out
--- very quickly as it gets further from the listener.
--- The default value of the attenuation is 1.
---
--- @param attenuation New attenuation factor of the sound
---
--- @see `getAttenuation`, `setMinDistance`
---@type fun(self: sf.Sound, attenuation: number)
sf.Sound.setAttenuation = function() end
--- @brief Set the effect processor to be applied to the sound
---
--- The effect processor is a callable that will be called
--- with sound data to be processed.
---
--- @param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing
---@type fun(self: sf.Sound, effectProcessor: sf.SoundSource.EffectProcessor|nil)
sf.Sound.setEffectProcessor = function() end
--- @brief Get the pitch of the sound
---
--- @return Pitch of the sound
---
--- @see `setPitch`
---@type fun(self: sf.Sound): number
sf.Sound.getPitch = function() end
--- @brief Get the pan of the sound
---
--- @return Pan of the sound
---
--- @see `setPan`
---@type fun(self: sf.Sound): number
sf.Sound.getPan = function() end
--- @brief Get the volume of the sound
---
--- @return Volume of the sound, in the range [0, 100]
---
--- @see `setVolume`
---@type fun(self: sf.Sound): number
sf.Sound.getVolume = function() end
--- @brief Tell whether spatialization of the sound is enabled
---
--- @return `true` if spatialization is enabled, `false` if it's disabled
---
--- @see `setSpatializationEnabled`
---@type fun(self: sf.Sound): boolean
sf.Sound.isSpatializationEnabled = function() end
--- @brief Get the 3D position of the sound in the audio scene
---
--- @return Position of the sound
---
--- @see `setPosition`
---@type fun(self: sf.Sound): sf.Vector3f
sf.Sound.getPosition = function() end
--- @brief Get the 3D direction of the sound in the audio scene
---
--- @return Direction of the sound
---
--- @see `setDirection`
---@type fun(self: sf.Sound): sf.Vector3f
sf.Sound.getDirection = function() end
--- @brief Get the cone properties of the sound in the audio scene
---
--- @return Cone properties of the sound
---
--- @see `setCone`
---@type fun(self: sf.Sound): sf.SoundSource.Cone
sf.Sound.getCone = function() end
--- @brief Get the 3D velocity of the sound in the audio scene
---
--- @return Velocity of the sound
---
--- @see `setVelocity`
---@type fun(self: sf.Sound): sf.Vector3f
sf.Sound.getVelocity = function() end
--- @brief Get the doppler factor of the sound
---
--- @return Doppler factor of the sound
---
--- @see `setDopplerFactor`
---@type fun(self: sf.Sound): number
sf.Sound.getDopplerFactor = function() end
--- @brief Get the directional attenuation factor of the sound
---
--- @return Directional attenuation factor of the sound
---
--- @see `setDirectionalAttenuationFactor`
---@type fun(self: sf.Sound): number
sf.Sound.getDirectionalAttenuationFactor = function() end
--- @brief Tell whether the sound's position is relative to the
--- listener or is absolute
---
--- @return `true` if the position is relative, `false` if it's absolute
---
--- @see `setRelativeToListener`
---@type fun(self: sf.Sound): boolean
sf.Sound.isRelativeToListener = function() end
--- @brief Get the minimum distance of the sound
---
--- @return Minimum distance of the sound
---
--- @see `setMinDistance`, `getAttenuation`
---@type fun(self: sf.Sound): number
sf.Sound.getMinDistance = function() end
--- @brief Get the maximum distance of the sound
---
--- @return Maximum distance of the sound
---
--- @see `setMaxDistance`, `getAttenuation`
---@type fun(self: sf.Sound): number
sf.Sound.getMaxDistance = function() end
--- @brief Get the minimum gain of the sound
---
--- @return Minimum gain of the sound
---
--- @see `setMinGain`, `getAttenuation`
---@type fun(self: sf.Sound): number
sf.Sound.getMinGain = function() end
--- @brief Get the maximum gain of the sound
---
--- @return Maximum gain of the sound
---
--- @see `setMaxGain`, `getAttenuation`
---@type fun(self: sf.Sound): number
sf.Sound.getMaxGain = function() end
--- @brief Get the attenuation factor of the sound
---
--- @return Attenuation factor of the sound
---
--- @see `setAttenuation`, `getMinDistance`
---@type fun(self: sf.Sound): number
sf.Sound.getAttenuation = function() end
--- @brief Start or resume playing the sound
---
--- This function starts the stream if it was stopped, resumes
--- it if it was paused, and restarts it from beginning if it
--- was it already playing.
--- This function uses its own thread so that it doesn't block
--- the rest of the program while the sound is played.
---
--- @see `pause`, `stop`
---@type fun(self: sf.Sound)
sf.Sound.play = function() end
--- @brief Pause the sound
---
--- This function pauses the sound if it was playing,
--- otherwise (sound already paused or stopped) it has no effect.
---
--- @see `play`, `stop`
---@type fun(self: sf.Sound)
sf.Sound.pause = function() end
--- @brief stop playing the sound
---
--- This function stops the sound if it was playing or paused,
--- and does nothing if it was already stopped.
--- It also resets the playing position (unlike `pause()`).
---
--- @see `play`, `pause`
---@type fun(self: sf.Sound)
sf.Sound.stop = function() end
--- @brief Get the current status of the sound (stopped, paused, playing)
---
--- @return Current status of the sound
---@type fun(self: sf.Sound): sf.SoundSource.Status
sf.Sound.getStatus = function() end
--- @brief Set the source buffer containing the audio data to play
---
--- It is important to note that the sound buffer is not copied,
--- thus the `sf::SoundBuffer` instance must remain alive as long
--- as it is attached to the sound.
---
--- @param buffer Sound buffer to attach to the sound
---
--- @see `getBuffer`
---@type fun(self: sf.Sound, buffer: sf.SoundBuffer)
sf.Sound.setBuffer = function() end
--- @brief Set whether or not the sound should loop after reaching the end
---
--- If set, the sound will restart from beginning after
--- reaching the end and so on, until it is stopped or
--- `setLooping(false)` is called.
--- The default looping state for sound is `false`.
---
--- @param loop `true` to play in loop, `false` to play once
---
--- @see `isLooping`
---@type fun(self: sf.Sound, loop: boolean)
sf.Sound.setLooping = function() end
--- @brief Change the current playing position of the sound
---
--- The playing position can be changed when the sound is
--- either paused or playing. Changing the playing position
--- when the sound is stopped has no effect, since playing
--- the sound will reset its position.
---
--- @param timeOffset New playing position, from the beginning of the sound
---
--- @see `getPlayingOffset`
---@type fun(self: sf.Sound, timeOffset: sf.Time)
sf.Sound.setPlayingOffset = function() end
--- @brief Get the audio buffer attached to the sound
---
--- @return Sound buffer attached to the sound
---@type fun(self: sf.Sound): sf.SoundBuffer
sf.Sound.getBuffer = function() end
--- @brief Tell whether or not the sound is in loop mode
---
--- @return `true` if the sound is looping, `false` otherwise
---
--- @see `setLooping`
---@type fun(self: sf.Sound): boolean
sf.Sound.isLooping = function() end
--- @brief Get the current playing position of the sound
---
--- @return Current playing position, from the beginning of the sound
---
--- @see `setPlayingOffset`
---@type fun(self: sf.Sound): sf.Time
sf.Sound.getPlayingOffset = function() end
--- @brief Abstract base class for streamed audio sources
---@class sf.SoundStream : sf.SoundSource
sf.SoundStream = sf.SoundStream or {}
--- @brief Set the pitch of the sound
---
--- The pitch represents the perceived fundamental frequency
--- of a sound; thus you can make a sound more acute or grave
--- by changing its pitch. A side effect of changing the pitch
--- is to modify the playing speed of the sound as well.
--- The default value for the pitch is 1.
---
--- @param pitch New pitch to apply to the sound
---
--- @see `getPitch`
---@type fun(self: sf.SoundStream, pitch: number)
sf.SoundStream.setPitch = function() end
--- @brief Set the pan of the sound
---
--- Using panning, a mono sound can be panned between
--- stereo channels. When the pan is set to -1, the sound
--- is played only on the left channel, when the pan is set
--- to +1, the sound is played only on the right channel.
---
--- @param pan New pan to apply to the sound [-1, +1]
---
--- @see `getPan`
---@type fun(self: sf.SoundStream, pan: number)
sf.SoundStream.setPan = function() end
--- @brief Set the volume of the sound
---
--- The volume is a value between 0 (mute) and 100 (full volume).
--- The default value for the volume is 100.
---
--- @param volume Volume of the sound
---
--- @see `getVolume`
---@type fun(self: sf.SoundStream, volume: number)
sf.SoundStream.setVolume = function() end
--- @brief Set whether spatialization of the sound is enabled
---
--- Spatialization is the application of various effects to
--- simulate a sound being emitted at a virtual position in
--- 3D space and exhibiting various physical phenomena such as
--- directional attenuation and doppler shift.
---
--- @param enabled `true` to enable spatialization, `false` to disable
---
--- @see `isSpatializationEnabled`
---@type fun(self: sf.SoundStream, enabled: boolean)
sf.SoundStream.setSpatializationEnabled = function() end
--- @brief Set the 3D position of the sound in the audio scene
---
--- Only sounds with one channel (mono sounds) can be
--- spatialized.
--- The default position of a sound is (0, 0, 0).
---
--- @param position Position of the sound in the scene
---
--- @see `getPosition`
---@type fun(self: sf.SoundStream, position: sf.Vector3f)
sf.SoundStream.setPosition = function() end
--- @brief Set the 3D direction of the sound in the audio scene
---
--- The direction defines where the sound source is facing
--- in 3D space. It will affect how the sound is attenuated
--- if facing away from the listener.
--- The default direction of a sound is (0, 0, -1).
---
--- @param direction Direction of the sound in the scene
---
--- @see `getDirection`
---@type fun(self: sf.SoundStream, direction: sf.Vector3f)
sf.SoundStream.setDirection = function() end
--- @brief Set the cone properties of the sound in the audio scene
---
--- The cone defines how directional attenuation is applied.
--- The default cone of a sound is (2 * PI, 2 * PI, 1).
---
--- @param cone Cone properties of the sound in the scene
---
--- @see `getCone`
---@type fun(self: sf.SoundStream, cone: sf.SoundSource.Cone)
sf.SoundStream.setCone = function() end
--- @brief Set the 3D velocity of the sound in the audio scene
---
--- The velocity is used to determine how to doppler shift
--- the sound. Sounds moving towards the listener will be
--- perceived to have a higher pitch and sounds moving away
--- from the listener will be perceived to have a lower pitch.
---
--- @param velocity Velocity of the sound in the scene
---
--- @see `getVelocity`
---@type fun(self: sf.SoundStream, velocity: sf.Vector3f)
sf.SoundStream.setVelocity = function() end
--- @brief Set the doppler factor of the sound
---
--- The doppler factor determines how strong the doppler
--- shift will be.
---
--- @param factor New doppler factor to apply to the sound
---
--- @see `getDopplerFactor`
---@type fun(self: sf.SoundStream, factor: number)
sf.SoundStream.setDopplerFactor = function() end
--- @brief Set the directional attenuation factor of the sound
---
--- Depending on the virtual position of an output channel
--- relative to the listener (such as in surround sound
--- setups), sounds will be attenuated when emitting them
--- from certain channels. This factor determines how strong
--- the attenuation based on output channel position
--- relative to the listener is.
---
--- @param factor New directional attenuation factor to apply to the sound
---
--- @see `getDirectionalAttenuationFactor`
---@type fun(self: sf.SoundStream, factor: number)
sf.SoundStream.setDirectionalAttenuationFactor = function() end
--- @brief Make the sound's position relative to the listener or absolute
---
--- Making a sound relative to the listener will ensure that it will always
--- be played the same way regardless of the position of the listener.
--- This can be useful for non-spatialized sounds, sounds that are
--- produced by the listener, or sounds attached to it.
--- The default value is `false` (position is absolute).
---
--- @param relative `true` to set the position relative, `false` to set it absolute
---
--- @see `isRelativeToListener`
---@type fun(self: sf.SoundStream, relative: boolean)
sf.SoundStream.setRelativeToListener = function() end
--- @brief Set the minimum distance of the sound
---
--- The "minimum distance" of a sound is the maximum
--- distance at which it is heard at its maximum volume. Further
--- than the minimum distance, it will start to fade out according
--- to its attenuation factor. A value of 0 ("inside the head
--- of the listener") is an invalid value and is forbidden.
--- The default value of the minimum distance is 1.
---
--- @param distance New minimum distance of the sound
---
--- @see `getMinDistance`, `setAttenuation`
---@type fun(self: sf.SoundStream, distance: number)
sf.SoundStream.setMinDistance = function() end
--- @brief Set the maximum distance of the sound
---
--- The "maximum distance" of a sound is the minimum
--- distance at which it is heard at its minimum volume. Closer
--- than the maximum distance, it will start to fade in according
--- to its attenuation factor.
--- The default value of the maximum distance is the maximum
--- value a float can represent.
---
--- @param distance New maximum distance of the sound
---
--- @see `getMaxDistance`, `setAttenuation`
---@type fun(self: sf.SoundStream, distance: number)
sf.SoundStream.setMaxDistance = function() end
--- @brief Set the minimum gain of the sound
---
--- When the sound is further away from the listener than
--- the "maximum distance" the attenuated gain is clamped
--- so it cannot go below the minimum gain value.
---
--- @param gain New minimum gain of the sound
---
--- @see `getMinGain`, `setAttenuation`
---@type fun(self: sf.SoundStream, gain: number)
sf.SoundStream.setMinGain = function() end
--- @brief Set the maximum gain of the sound
---
--- When the sound is closer from the listener than
--- the "minimum distance" the attenuated gain is clamped
--- so it cannot go above the maximum gain value.
---
--- @param gain New maximum gain of the sound
---
--- @see `getMaxGain`, `setAttenuation`
---@type fun(self: sf.SoundStream, gain: number)
sf.SoundStream.setMaxGain = function() end
--- @brief Set the attenuation factor of the sound
---
--- The attenuation is a multiplicative factor which makes
--- the sound more or less loud according to its distance
--- from the listener. An attenuation of 0 will produce a
--- non-attenuated sound, i.e. its volume will always be the same
--- whether it is heard from near or from far. On the other hand,
--- an attenuation value such as 100 will make the sound fade out
--- very quickly as it gets further from the listener.
--- The default value of the attenuation is 1.
---
--- @param attenuation New attenuation factor of the sound
---
--- @see `getAttenuation`, `setMinDistance`
---@type fun(self: sf.SoundStream, attenuation: number)
sf.SoundStream.setAttenuation = function() end
--- @brief Set the effect processor to be applied to the sound
---
--- The effect processor is a callable that will be called
--- with sound data to be processed.
---
--- @param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing
---@type fun(self: sf.SoundStream, effectProcessor: sf.SoundSource.EffectProcessor|nil)
sf.SoundStream.setEffectProcessor = function() end
--- @brief Get the pitch of the sound
---
--- @return Pitch of the sound
---
--- @see `setPitch`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getPitch = function() end
--- @brief Get the pan of the sound
---
--- @return Pan of the sound
---
--- @see `setPan`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getPan = function() end
--- @brief Get the volume of the sound
---
--- @return Volume of the sound, in the range [0, 100]
---
--- @see `setVolume`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getVolume = function() end
--- @brief Tell whether spatialization of the sound is enabled
---
--- @return `true` if spatialization is enabled, `false` if it's disabled
---
--- @see `setSpatializationEnabled`
---@type fun(self: sf.SoundStream): boolean
sf.SoundStream.isSpatializationEnabled = function() end
--- @brief Get the 3D position of the sound in the audio scene
---
--- @return Position of the sound
---
--- @see `setPosition`
---@type fun(self: sf.SoundStream): sf.Vector3f
sf.SoundStream.getPosition = function() end
--- @brief Get the 3D direction of the sound in the audio scene
---
--- @return Direction of the sound
---
--- @see `setDirection`
---@type fun(self: sf.SoundStream): sf.Vector3f
sf.SoundStream.getDirection = function() end
--- @brief Get the cone properties of the sound in the audio scene
---
--- @return Cone properties of the sound
---
--- @see `setCone`
---@type fun(self: sf.SoundStream): sf.SoundSource.Cone
sf.SoundStream.getCone = function() end
--- @brief Get the 3D velocity of the sound in the audio scene
---
--- @return Velocity of the sound
---
--- @see `setVelocity`
---@type fun(self: sf.SoundStream): sf.Vector3f
sf.SoundStream.getVelocity = function() end
--- @brief Get the doppler factor of the sound
---
--- @return Doppler factor of the sound
---
--- @see `setDopplerFactor`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getDopplerFactor = function() end
--- @brief Get the directional attenuation factor of the sound
---
--- @return Directional attenuation factor of the sound
---
--- @see `setDirectionalAttenuationFactor`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getDirectionalAttenuationFactor = function() end
--- @brief Tell whether the sound's position is relative to the
--- listener or is absolute
---
--- @return `true` if the position is relative, `false` if it's absolute
---
--- @see `setRelativeToListener`
---@type fun(self: sf.SoundStream): boolean
sf.SoundStream.isRelativeToListener = function() end
--- @brief Get the minimum distance of the sound
---
--- @return Minimum distance of the sound
---
--- @see `setMinDistance`, `getAttenuation`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getMinDistance = function() end
--- @brief Get the maximum distance of the sound
---
--- @return Maximum distance of the sound
---
--- @see `setMaxDistance`, `getAttenuation`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getMaxDistance = function() end
--- @brief Get the minimum gain of the sound
---
--- @return Minimum gain of the sound
---
--- @see `setMinGain`, `getAttenuation`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getMinGain = function() end
--- @brief Get the maximum gain of the sound
---
--- @return Maximum gain of the sound
---
--- @see `setMaxGain`, `getAttenuation`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getMaxGain = function() end
--- @brief Get the attenuation factor of the sound
---
--- @return Attenuation factor of the sound
---
--- @see `setAttenuation`, `getMinDistance`
---@type fun(self: sf.SoundStream): number
sf.SoundStream.getAttenuation = function() end
--- @brief Start or resume playing the audio stream
---
--- This function starts the stream if it was stopped, resumes
--- it if it was paused, and restarts it from the beginning if
--- it was already playing.
--- This function uses its own thread so that it doesn't block
--- the rest of the program while the stream is played.
---
--- @see `pause`, `stop`
---@type fun(self: sf.SoundStream)
sf.SoundStream.play = function() end
--- @brief Pause the audio stream
---
--- This function pauses the stream if it was playing,
--- otherwise (stream already paused or stopped) it has no effect.
---
--- @see `play`, `stop`
---@type fun(self: sf.SoundStream)
sf.SoundStream.pause = function() end
--- @brief Stop playing the audio stream
---
--- This function stops the stream if it was playing or paused,
--- and does nothing if it was already stopped.
--- It also resets the playing position (unlike `pause()`).
---
--- @see `play`, `pause`
---@type fun(self: sf.SoundStream)
sf.SoundStream.stop = function() end
--- @brief Get the current status of the stream (stopped, paused, playing)
---
--- @return Current status
---@type fun(self: sf.SoundStream): sf.SoundSource.Status
sf.SoundStream.getStatus = function() end
--- @brief Return the number of channels of the stream
---
--- 1 channel means a mono sound, 2 means stereo, etc.
---
--- @return Number of channels
---@type fun(self: sf.SoundStream): integer
sf.SoundStream.getChannelCount = function() end
--- @brief Get the stream sample rate of the stream
---
--- The sample rate is the number of audio samples played per
--- second. The higher, the better the quality.
---
--- @return Sample rate, in number of samples per second
---@type fun(self: sf.SoundStream): integer
sf.SoundStream.getSampleRate = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---@type fun(self: sf.SoundStream): sf.SoundChannel[]
sf.SoundStream.getChannelMap = function() end
--- @brief Change the current playing position of the stream
---
--- The playing position can be changed when the stream is
--- either paused or playing. Changing the playing position
--- when the stream is stopped has no effect, since playing
--- the stream would reset its position.
---
--- @param timeOffset New playing position, from the beginning of the stream
---
--- @see `getPlayingOffset`
---@type fun(self: sf.SoundStream, timeOffset: sf.Time)
sf.SoundStream.setPlayingOffset = function() end
--- @brief Get the current playing position of the stream
---
--- @return Current playing position, from the beginning of the stream
---
--- @see `setPlayingOffset`
---@type fun(self: sf.SoundStream): sf.Time
sf.SoundStream.getPlayingOffset = function() end
--- @brief Set whether or not the stream should loop after reaching the end
---
--- If set, the stream will restart from beginning after
--- reaching the end and so on, until it is stopped or
--- `setLooping(false)` is called.
--- The default looping state for streams is `false`.
---
--- @param loop `true` to play in loop, `false` to play once
---
--- @see `isLooping`
---@type fun(self: sf.SoundStream, loop: boolean)
sf.SoundStream.setLooping = function() end
--- @brief Tell whether or not the stream is in loop mode
---
--- @return `true` if the stream is looping, `false` otherwise
---
--- @see `setLooping`
---@type fun(self: sf.SoundStream): boolean
sf.SoundStream.isLooping = function() end
--- @brief Structure defining a chunk of audio data to stream
---@class sf.SoundStream.Chunk
--- Pointer to the audio samples
---@field samples integer
--- Number of samples pointed by Samples
---@field sampleCount integer
sf.SoundStream.Chunk = sf.SoundStream.Chunk or {}
---@type fun(): sf.SoundStream.Chunk
sf.SoundStream.Chunk.new = function() end
--- @brief Streamed music played from an audio file
---@class sf.Music : sf.SoundStream, sf.SoundSource
sf.Music = sf.Music or {}
--- @brief Construct a music from an audio file
---
--- This function doesn't start playing the music (call `play()`
--- to do so).
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @warning Since the music is not loaded at once but rather
--- streamed continuously, the file must remain accessible until
--- the `sf::Music` object loads a new music or is destroyed.
---
--- @param filename Path of the music file to open
---
--- @throws sf::Exception if loading was unsuccessful
---
--- @see `openFromMemory`, `openFromStream`
---@overload fun(stream: sf.InputStream): sf.Music
---@overload fun(): sf.Music
---@overload fun(data: any): sf.Music
---@param filename string
---@return sf.Music
function sf.Music.new(filename) end
--- @brief Set the pitch of the sound
---
--- The pitch represents the perceived fundamental frequency
--- of a sound; thus you can make a sound more acute or grave
--- by changing its pitch. A side effect of changing the pitch
--- is to modify the playing speed of the sound as well.
--- The default value for the pitch is 1.
---
--- @param pitch New pitch to apply to the sound
---
--- @see `getPitch`
---@type fun(self: sf.Music, pitch: number)
sf.Music.setPitch = function() end
--- @brief Set the pan of the sound
---
--- Using panning, a mono sound can be panned between
--- stereo channels. When the pan is set to -1, the sound
--- is played only on the left channel, when the pan is set
--- to +1, the sound is played only on the right channel.
---
--- @param pan New pan to apply to the sound [-1, +1]
---
--- @see `getPan`
---@type fun(self: sf.Music, pan: number)
sf.Music.setPan = function() end
--- @brief Set the volume of the sound
---
--- The volume is a value between 0 (mute) and 100 (full volume).
--- The default value for the volume is 100.
---
--- @param volume Volume of the sound
---
--- @see `getVolume`
---@type fun(self: sf.Music, volume: number)
sf.Music.setVolume = function() end
--- @brief Set whether spatialization of the sound is enabled
---
--- Spatialization is the application of various effects to
--- simulate a sound being emitted at a virtual position in
--- 3D space and exhibiting various physical phenomena such as
--- directional attenuation and doppler shift.
---
--- @param enabled `true` to enable spatialization, `false` to disable
---
--- @see `isSpatializationEnabled`
---@type fun(self: sf.Music, enabled: boolean)
sf.Music.setSpatializationEnabled = function() end
--- @brief Set the 3D position of the sound in the audio scene
---
--- Only sounds with one channel (mono sounds) can be
--- spatialized.
--- The default position of a sound is (0, 0, 0).
---
--- @param position Position of the sound in the scene
---
--- @see `getPosition`
---@type fun(self: sf.Music, position: sf.Vector3f)
sf.Music.setPosition = function() end
--- @brief Set the 3D direction of the sound in the audio scene
---
--- The direction defines where the sound source is facing
--- in 3D space. It will affect how the sound is attenuated
--- if facing away from the listener.
--- The default direction of a sound is (0, 0, -1).
---
--- @param direction Direction of the sound in the scene
---
--- @see `getDirection`
---@type fun(self: sf.Music, direction: sf.Vector3f)
sf.Music.setDirection = function() end
--- @brief Set the cone properties of the sound in the audio scene
---
--- The cone defines how directional attenuation is applied.
--- The default cone of a sound is (2 * PI, 2 * PI, 1).
---
--- @param cone Cone properties of the sound in the scene
---
--- @see `getCone`
---@type fun(self: sf.Music, cone: sf.SoundSource.Cone)
sf.Music.setCone = function() end
--- @brief Set the 3D velocity of the sound in the audio scene
---
--- The velocity is used to determine how to doppler shift
--- the sound. Sounds moving towards the listener will be
--- perceived to have a higher pitch and sounds moving away
--- from the listener will be perceived to have a lower pitch.
---
--- @param velocity Velocity of the sound in the scene
---
--- @see `getVelocity`
---@type fun(self: sf.Music, velocity: sf.Vector3f)
sf.Music.setVelocity = function() end
--- @brief Set the doppler factor of the sound
---
--- The doppler factor determines how strong the doppler
--- shift will be.
---
--- @param factor New doppler factor to apply to the sound
---
--- @see `getDopplerFactor`
---@type fun(self: sf.Music, factor: number)
sf.Music.setDopplerFactor = function() end
--- @brief Set the directional attenuation factor of the sound
---
--- Depending on the virtual position of an output channel
--- relative to the listener (such as in surround sound
--- setups), sounds will be attenuated when emitting them
--- from certain channels. This factor determines how strong
--- the attenuation based on output channel position
--- relative to the listener is.
---
--- @param factor New directional attenuation factor to apply to the sound
---
--- @see `getDirectionalAttenuationFactor`
---@type fun(self: sf.Music, factor: number)
sf.Music.setDirectionalAttenuationFactor = function() end
--- @brief Make the sound's position relative to the listener or absolute
---
--- Making a sound relative to the listener will ensure that it will always
--- be played the same way regardless of the position of the listener.
--- This can be useful for non-spatialized sounds, sounds that are
--- produced by the listener, or sounds attached to it.
--- The default value is `false` (position is absolute).
---
--- @param relative `true` to set the position relative, `false` to set it absolute
---
--- @see `isRelativeToListener`
---@type fun(self: sf.Music, relative: boolean)
sf.Music.setRelativeToListener = function() end
--- @brief Set the minimum distance of the sound
---
--- The "minimum distance" of a sound is the maximum
--- distance at which it is heard at its maximum volume. Further
--- than the minimum distance, it will start to fade out according
--- to its attenuation factor. A value of 0 ("inside the head
--- of the listener") is an invalid value and is forbidden.
--- The default value of the minimum distance is 1.
---
--- @param distance New minimum distance of the sound
---
--- @see `getMinDistance`, `setAttenuation`
---@type fun(self: sf.Music, distance: number)
sf.Music.setMinDistance = function() end
--- @brief Set the maximum distance of the sound
---
--- The "maximum distance" of a sound is the minimum
--- distance at which it is heard at its minimum volume. Closer
--- than the maximum distance, it will start to fade in according
--- to its attenuation factor.
--- The default value of the maximum distance is the maximum
--- value a float can represent.
---
--- @param distance New maximum distance of the sound
---
--- @see `getMaxDistance`, `setAttenuation`
---@type fun(self: sf.Music, distance: number)
sf.Music.setMaxDistance = function() end
--- @brief Set the minimum gain of the sound
---
--- When the sound is further away from the listener than
--- the "maximum distance" the attenuated gain is clamped
--- so it cannot go below the minimum gain value.
---
--- @param gain New minimum gain of the sound
---
--- @see `getMinGain`, `setAttenuation`
---@type fun(self: sf.Music, gain: number)
sf.Music.setMinGain = function() end
--- @brief Set the maximum gain of the sound
---
--- When the sound is closer from the listener than
--- the "minimum distance" the attenuated gain is clamped
--- so it cannot go above the maximum gain value.
---
--- @param gain New maximum gain of the sound
---
--- @see `getMaxGain`, `setAttenuation`
---@type fun(self: sf.Music, gain: number)
sf.Music.setMaxGain = function() end
--- @brief Set the attenuation factor of the sound
---
--- The attenuation is a multiplicative factor which makes
--- the sound more or less loud according to its distance
--- from the listener. An attenuation of 0 will produce a
--- non-attenuated sound, i.e. its volume will always be the same
--- whether it is heard from near or from far. On the other hand,
--- an attenuation value such as 100 will make the sound fade out
--- very quickly as it gets further from the listener.
--- The default value of the attenuation is 1.
---
--- @param attenuation New attenuation factor of the sound
---
--- @see `getAttenuation`, `setMinDistance`
---@type fun(self: sf.Music, attenuation: number)
sf.Music.setAttenuation = function() end
--- @brief Set the effect processor to be applied to the sound
---
--- The effect processor is a callable that will be called
--- with sound data to be processed.
---
--- @param effectProcessor The effect processor to attach to this sound, attach an empty processor to disable processing
---@type fun(self: sf.Music, effectProcessor: sf.SoundSource.EffectProcessor|nil)
sf.Music.setEffectProcessor = function() end
--- @brief Get the pitch of the sound
---
--- @return Pitch of the sound
---
--- @see `setPitch`
---@type fun(self: sf.Music): number
sf.Music.getPitch = function() end
--- @brief Get the pan of the sound
---
--- @return Pan of the sound
---
--- @see `setPan`
---@type fun(self: sf.Music): number
sf.Music.getPan = function() end
--- @brief Get the volume of the sound
---
--- @return Volume of the sound, in the range [0, 100]
---
--- @see `setVolume`
---@type fun(self: sf.Music): number
sf.Music.getVolume = function() end
--- @brief Tell whether spatialization of the sound is enabled
---
--- @return `true` if spatialization is enabled, `false` if it's disabled
---
--- @see `setSpatializationEnabled`
---@type fun(self: sf.Music): boolean
sf.Music.isSpatializationEnabled = function() end
--- @brief Get the 3D position of the sound in the audio scene
---
--- @return Position of the sound
---
--- @see `setPosition`
---@type fun(self: sf.Music): sf.Vector3f
sf.Music.getPosition = function() end
--- @brief Get the 3D direction of the sound in the audio scene
---
--- @return Direction of the sound
---
--- @see `setDirection`
---@type fun(self: sf.Music): sf.Vector3f
sf.Music.getDirection = function() end
--- @brief Get the cone properties of the sound in the audio scene
---
--- @return Cone properties of the sound
---
--- @see `setCone`
---@type fun(self: sf.Music): sf.SoundSource.Cone
sf.Music.getCone = function() end
--- @brief Get the 3D velocity of the sound in the audio scene
---
--- @return Velocity of the sound
---
--- @see `setVelocity`
---@type fun(self: sf.Music): sf.Vector3f
sf.Music.getVelocity = function() end
--- @brief Get the doppler factor of the sound
---
--- @return Doppler factor of the sound
---
--- @see `setDopplerFactor`
---@type fun(self: sf.Music): number
sf.Music.getDopplerFactor = function() end
--- @brief Get the directional attenuation factor of the sound
---
--- @return Directional attenuation factor of the sound
---
--- @see `setDirectionalAttenuationFactor`
---@type fun(self: sf.Music): number
sf.Music.getDirectionalAttenuationFactor = function() end
--- @brief Tell whether the sound's position is relative to the
--- listener or is absolute
---
--- @return `true` if the position is relative, `false` if it's absolute
---
--- @see `setRelativeToListener`
---@type fun(self: sf.Music): boolean
sf.Music.isRelativeToListener = function() end
--- @brief Get the minimum distance of the sound
---
--- @return Minimum distance of the sound
---
--- @see `setMinDistance`, `getAttenuation`
---@type fun(self: sf.Music): number
sf.Music.getMinDistance = function() end
--- @brief Get the maximum distance of the sound
---
--- @return Maximum distance of the sound
---
--- @see `setMaxDistance`, `getAttenuation`
---@type fun(self: sf.Music): number
sf.Music.getMaxDistance = function() end
--- @brief Get the minimum gain of the sound
---
--- @return Minimum gain of the sound
---
--- @see `setMinGain`, `getAttenuation`
---@type fun(self: sf.Music): number
sf.Music.getMinGain = function() end
--- @brief Get the maximum gain of the sound
---
--- @return Maximum gain of the sound
---
--- @see `setMaxGain`, `getAttenuation`
---@type fun(self: sf.Music): number
sf.Music.getMaxGain = function() end
--- @brief Get the attenuation factor of the sound
---
--- @return Attenuation factor of the sound
---
--- @see `setAttenuation`, `getMinDistance`
---@type fun(self: sf.Music): number
sf.Music.getAttenuation = function() end
--- @brief Start or resume playing the sound source
---
--- This function starts the source if it was stopped, resumes
--- it if it was paused, and restarts it from the beginning if
--- it was already playing.
---
--- @see `pause`, `stop`
---@type fun(self: sf.Music)
sf.Music.play = function() end
--- @brief Pause the sound source
---
--- This function pauses the source if it was playing,
--- otherwise (source already paused or stopped) it has no effect.
---
--- @see `play`, `stop`
---@type fun(self: sf.Music)
sf.Music.pause = function() end
--- @brief Stop playing the sound source
---
--- This function stops the source if it was playing or paused,
--- and does nothing if it was already stopped.
--- It also resets the playing position (unlike `pause()`).
---
--- @see `play`, `pause`
---@type fun(self: sf.Music)
sf.Music.stop = function() end
--- @brief Get the current status of the sound (stopped, paused, playing)
---
--- @return Current status of the sound
---@type fun(self: sf.Music): sf.SoundSource.Status
sf.Music.getStatus = function() end
--- @brief Return the number of channels of the stream
---
--- 1 channel means a mono sound, 2 means stereo, etc.
---
--- @return Number of channels
---@type fun(self: sf.Music): integer
sf.Music.getChannelCount = function() end
--- @brief Get the stream sample rate of the stream
---
--- The sample rate is the number of audio samples played per
--- second. The higher, the better the quality.
---
--- @return Sample rate, in number of samples per second
---@type fun(self: sf.Music): integer
sf.Music.getSampleRate = function() end
--- @brief Get the map of position in sample frame to sound channel
---
--- This is used to map a sample in the sample stream to a
--- position during spatialization.
---
--- @return Map of position in sample frame to sound channel
---@type fun(self: sf.Music): sf.SoundChannel[]
sf.Music.getChannelMap = function() end
--- @brief Change the current playing position of the stream
---
--- The playing position can be changed when the stream is
--- either paused or playing. Changing the playing position
--- when the stream is stopped has no effect, since playing
--- the stream would reset its position.
---
--- @param timeOffset New playing position, from the beginning of the stream
---
--- @see `getPlayingOffset`
---@type fun(self: sf.Music, timeOffset: sf.Time)
sf.Music.setPlayingOffset = function() end
--- @brief Get the current playing position of the stream
---
--- @return Current playing position, from the beginning of the stream
---
--- @see `setPlayingOffset`
---@type fun(self: sf.Music): sf.Time
sf.Music.getPlayingOffset = function() end
--- @brief Set whether or not the stream should loop after reaching the end
---
--- If set, the stream will restart from beginning after
--- reaching the end and so on, until it is stopped or
--- `setLooping(false)` is called.
--- The default looping state for streams is `false`.
---
--- @param loop `true` to play in loop, `false` to play once
---
--- @see `isLooping`
---@type fun(self: sf.Music, loop: boolean)
sf.Music.setLooping = function() end
--- @brief Tell whether or not the stream is in loop mode
---
--- @return `true` if the stream is looping, `false` otherwise
---
--- @see `setLooping`
---@type fun(self: sf.Music): boolean
sf.Music.isLooping = function() end
--- @brief Open a music from an audio file
---
--- This function doesn't start playing the music (call `play()`
--- to do so).
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @warning Since the music is not loaded at once but rather
--- streamed continuously, the file must remain accessible until
--- the `sf::Music` object loads a new music or is destroyed.
---
--- @param filename Path of the music file to open
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `openFromMemory`, `openFromStream`
---@type fun(self: sf.Music, filename: string): boolean
sf.Music.openFromFile = function() end
--- @brief Open a music from an audio file in memory
---
--- This function doesn't start playing the music (call `play()`
--- to do so).
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @warning Since the music is not loaded at once but rather streamed
--- continuously, the `data` buffer must remain accessible until
--- the `sf::Music` object loads a new music or is destroyed. That is,
--- you can't deallocate the buffer right after calling this function.
---
--- @param data        Pointer to the file data in memory
--- @param sizeInBytes Size of the data to load, in bytes
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `openFromFile`, `openFromStream`
---@type fun(self: sf.Music, data: any): boolean
sf.Music.openFromMemory = function() end
--- @brief Open a music from an audio file in a custom stream
---
--- This function doesn't start playing the music (call `play()`
--- to do so).
--- See the documentation of `sf::InputSoundFile` for the list
--- of supported formats.
---
--- @warning Since the music is not loaded at once but rather
--- streamed continuously, the `stream` must remain accessible
--- until the `sf::Music` object loads a new music or is destroyed.
---
--- @param stream Source stream to read from
---
--- @return `true` if loading succeeded, `false` if it failed
---
--- @see `openFromFile`, `openFromMemory`
---@type fun(self: sf.Music, stream: sf.InputStream): boolean
sf.Music.openFromStream = function() end
--- @brief Get the total duration of the music
---
--- @return Music duration
---@type fun(self: sf.Music): sf.Time
sf.Music.getDuration = function() end
--- @brief Get the positions of the of the sound's looping sequence
---
--- @return Loop Time position class.
---
--- @warning Since `setLoopPoints()` performs some adjustments on the
--- provided values and rounds them to internal samples, a call to
--- `getLoopPoints()` is not guaranteed to return the same times passed
--- into a previous call to `setLoopPoints()`. However, it is guaranteed
--- to return times that will map to the valid internal samples of
--- this Music if they are later passed to `setLoopPoints()`.
---
--- @see `setLoopPoints`
---@type fun(self: sf.Music): sf.Music.TimeSpan
sf.Music.getLoopPoints = function() end
--- @brief Sets the beginning and duration of the sound's looping sequence using `sf::Time`
---
--- `setLoopPoints()` allows for specifying the beginning offset and the duration of the loop such that,
--- when the music is enabled for looping, it will seamlessly seek to the beginning whenever it
--- encounters the end of the duration. Valid ranges for `timePoints.offset` and `timePoints.length` are
--- [0, Dur) and (0, Dur-offset] respectively, where Dur is the value returned by `getDuration()`.
--- Note that the EOF "loop point" from the end to the beginning of the stream is still honored,
--- in case the caller seeks to a point after the end of the loop range. This function can be
--- safely called at any point after a stream is opened, and will be applied to a playing sound
--- without affecting the current playing offset.
---
--- @warning Setting the loop points while the stream's status is Paused
--- will set its status to Stopped. The playing offset will be unaffected.
---
--- @param timePoints The definition of the loop. Can be any time points within the sound's length
---
--- @see `getLoopPoints`
---@type fun(self: sf.Music, timePoints: sf.Music.TimeSpan)
sf.Music.setLoopPoints = function() end
--- @brief Structure defining a time range using the template type
---@class sf.Music.TimeSpan
--- The beginning offset of the time range
---@field offset sf.Time
--- The length of the time range
---@field length sf.Time
sf.Music.TimeSpan = sf.Music.TimeSpan or {}
---@type fun(): sf.Music.TimeSpan
sf.Music.TimeSpan.new = function() end
--- @brief Encapsulate an IPv4 network address
---@class sf.IpAddress
sf.IpAddress = sf.IpAddress or {}
--- @brief Construct an IPv4 address from 4 bytes
---
--- Calling `IpAddress(a, b, c, d)` is equivalent to calling
--- `IpAddress::resolve("a.b.c.d")`, but safer as it doesn't
--- have to parse a string to get the address components.
---
--- @param byte0 First byte of the address
--- @param byte1 Second byte of the address
--- @param byte2 Third byte of the address
--- @param byte3 Fourth byte of the address
---@overload fun(address: integer): sf.IpAddress
---@overload fun(bytes: any): sf.IpAddress
---@param byte0 integer
---@param byte1 integer
---@param byte2 integer
---@param byte3 integer
---@return sf.IpAddress
function sf.IpAddress.new(byte0, byte1, byte2, byte3) end
--- @brief Construct the address from a null-terminated string view
---
--- @deprecated Use `sf::Dns::resolve()` instead.
---
--- Here @a address can be either a decimal address
--- (ex: "192.168.1.56") or a network name (ex: "localhost").
---
--- This function will only resolve to an IPv4 address.
--- Use Dns::resolve() to resolve to IPv6 addresses as well.
---
--- @param address IP address or network name
---
--- @return Address if provided argument was valid, otherwise `std::nullopt`
---@type fun(address: string): sf.IpAddress|nil
sf.IpAddress.resolve = function() end
--- @brief Try to construct an address from its string representation
---
--- The string should contain either a valid representation of an
--- IPv4 address in dotted-decimal notation or a valid representation
--- of an IPv6 address in internet standard notation.
---
--- Examples:
--- - 192.168.1.56
--- - FEDC:BA98:7654:3210:FEDC:BA98:7654:3210
--- - fedc:ba98:7654:3210:fedc:ba98:7654:3210
--- - 1080:0:0:0:8:800:200C:417A
--- - 1080::8:800:200C:417A
--- - FF01::101
--- - ::1
--- - ::
--- - 0:0:0:0:0:0:13.1.68.3
--- - ::13.1.68.3
--- - 0:0:0:0:0:FFFF:129.144.52.38
--- - ::FFFF:129.144.52.38
---
--- @param address String representation of the address
---
--- @return Address if provided argument was a valid string represenation of an IP address, otherwise `std::nullopt`
---
--- @see `toString`
---@type fun(address: string): sf.IpAddress|nil
sf.IpAddress.fromString = function() end
--- @brief Get a string representation of the address
---
--- The returned string is the decimal representation of the
--- IP address (like "192.168.1.56" or "FF01::101"), even if
--- it was constructed from a host name.
---
--- @return String representation of the address
---
--- @see `fromString`, `toInteger`
---@type fun(self: sf.IpAddress): string
sf.IpAddress.toString = function() end
--- @brief Get an integer representation of the address
---
--- This function can only be called if this is an IPv4
--- address. Check with isV4() before calling this function.
---
--- The returned number is the internal representation of the
--- address, and should be used for optimization purposes only
--- (like sending the address through a socket).
--- The integer produced by this function can then be converted
--- back to a `sf::IpAddress` with the proper constructor.
---
--- @return 32-bits unsigned integer representation of the address
---
--- @see `toString`
---@type fun(self: sf.IpAddress): integer
sf.IpAddress.toInteger = function() end
--- @brief Get an array of bytes representing the address
---
--- This function can only be called if this is an IPv6
--- address. Check with isV6() before calling this function.
---
--- The returned array is the internal representation of the
--- address, and should be used for optimization purposes only
--- (like sending the address through a socket).
--- The array produced by this function can then be converted
--- back to a `sf::IpAddress` with the proper constructor.
---
--- @return 16-byte array representation of the address
---
--- @see `toString`
---@type fun(self: sf.IpAddress): any
sf.IpAddress.toBytes = function() end
--- @brief Get the type of this IP address
---
--- @return The type of this IP address (IPv4 or IPv6)
---@type fun(self: sf.IpAddress): sf.IpAddress.Type
sf.IpAddress.getType = function() end
--- @brief Check if this IP address is an IPv4 address
---
--- Equivalent to getType() == Type::IPv4
---
--- @return true if this is an IPv4 address, false otherwise
---@type fun(self: sf.IpAddress): boolean
sf.IpAddress.isV4 = function() end
--- @brief Check if this IP address is an IPv6 address
---
--- Equivalent to getType() == Type::IPv6
---
--- @return true if this is an IPv6 address, false otherwise
---@type fun(self: sf.IpAddress): boolean
sf.IpAddress.isV6 = function() end
--- @brief Get the computer's local address
---
--- The local address is the address of the computer from the
--- LAN point of view, i.e. something like 192.168.1.56. It is
--- meaningful only for communications over the local network.
--- Unlike getPublicAddress, this function is fast and may be
--- used safely anywhere.
---
--- @param type Type of local address
---
--- @return Local IP address of the computer on success, `std::nullopt` otherwise
---
--- @see `getPublicAddress`
---@overload fun(): sf.IpAddress|nil
---@param type sf.IpAddress.Type
---@return sf.IpAddress|nil
function sf.IpAddress.getLocalAddress(type) end
--- @brief Get the computer's public address
---
--- The public address is the address of the computer from the
--- point of view of the internet, i.e. something like 89.54.1.169
--- or 2600:1901:0:13e0::1 as opposed to a private or local address
--- like 192.168.1.56 or fe80::1234:5678:9abc.
--- It is necessary for communication with hosts outside of the
--- local network.
---
--- The only way to reliably get the public address is to send
--- data to a host on the internet and see what the origin
--- address is; as a consequence, this function depends on both
--- your network connection and the server, and may be very slow.
--- You should try to use it as little as possible. Because this
--- function depends on the network connection and on a distant
--- server, you can specify a time limit if you don't want your
--- program to get stuck waiting in case there is a problem; this
--- limit is deactivated by default.
---
--- If tamper resistance is required, setting `secure` to `true`
--- will make use of verified HTTPS connections to get the address.
---
--- @param timeout Maximum time to wait
--- @param type    The type of public address to get, `std::nullopt` to specify no preference
--- @param secure  true to retrieve the public address via a secure HTTPS connection, false to retrieve via DNS or an insecure connection
---
--- @return Public IP address of the computer on success, `std::nullopt` otherwise
---
--- @see `getLocalAddress`
---@overload fun(): sf.IpAddress|nil
---@overload fun(timeout: sf.Time, type: sf.IpAddress.Type|nil, secure: boolean): sf.IpAddress|nil
---@overload fun(timeout: sf.Time, type: sf.IpAddress.Type|nil): sf.IpAddress|nil
---@param timeout sf.Time
---@return sf.IpAddress|nil
function sf.IpAddress.getPublicAddress(timeout) end
--- @brief Type of IP address
---@class sf.IpAddress.Type
--- IPv4 address
---@field IpV4 sf.IpAddress.Type
--- IPv6 address
---@field IpV6 sf.IpAddress.Type
sf.IpAddress.Type = sf.IpAddress.Type or {}
--- The same as AnyV4
---@type sf.IpAddress
sf.IpAddress.Any = nil
--- The same as LocalHostV4
---@type sf.IpAddress
sf.IpAddress.LocalHost = nil
--- The same as BroadcastV4
---@type sf.IpAddress
sf.IpAddress.Broadcast = nil
--- Value representing any IPv4 address (0.0.0.0)
---@type sf.IpAddress
sf.IpAddress.AnyV4 = nil
--- The "localhost" IPv4 address (for connecting a computer to itself locally)
---@type sf.IpAddress
sf.IpAddress.LocalHostV4 = nil
--- The "broadcast" IPv4 address (for sending UDP messages to everyone on a local network)
---@type sf.IpAddress
sf.IpAddress.BroadcastV4 = nil
--- Value representing any IPv6 address (::)
---@type sf.IpAddress
sf.IpAddress.AnyV6 = nil
--- The "localhost" IPv6 address (for connecting a computer to itself locally)
---@type sf.IpAddress
sf.IpAddress.LocalHostV6 = nil
--- @brief A DNS MX record
---@class sf.Dns.MxRecord
--- Host willing to act as mail exchange
---@field exchange string
--- Preference of this record among others, lower values are preferred
---@field preference integer
sf.Dns = sf.Dns or {}
sf.Dns.MxRecord = sf.Dns.MxRecord or {}
---@type fun(): sf.Dns.MxRecord
sf.Dns.MxRecord.new = function() end
--- @brief A DNS SRV record
---@class sf.Dns.SrvRecord
--- The domain name of the target host
---@field target string
--- The port on the target host of the service
---@field port integer
--- Server selection mechanism, larger weights should be given a proportionately higher probability of being selected
---@field weight integer
--- The priority of the target host, a client must attempt to contact the target host with the lowest-numbered priority it can reach
---@field priority integer
sf.Dns.SrvRecord = sf.Dns.SrvRecord or {}
---@type fun(): sf.Dns.SrvRecord
sf.Dns.SrvRecord.new = function() end
--- @brief Resolve a hostname into a list of IP addresses
---
--- @param hostname Hostname to resolve
--- @param servers  The list of servers to query, if empty use the default servers
--- @param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever
---
--- @return List of IP addresses the given hostname resolves to, `std::nullopt` is returned if name resolution fails, an empty list is returned if the hostname could not be resolved to any address
---@overload fun(hostname: string, servers: sf.IpAddress[]): sf.IpAddress[]|nil
---@overload fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.IpAddress[]|nil
---@param hostname string
---@return sf.IpAddress[]|nil
function sf.Dns.resolve(hostname) end
--- @brief Query NS records for a hostname
---
--- @param hostname Hostname to query NS records for
--- @param servers  The list of servers to query, if empty use the default servers
--- @param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever
---
--- @return List of NS record strings, an empty list is returned if there are no NS records for the hostname
---@overload fun(hostname: string, servers: sf.IpAddress[]): string[]
---@overload fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): string[]
---@param hostname string
---@return string[]
function sf.Dns.queryNs(hostname) end
--- @brief Query MX records for a hostname
---
--- @param hostname Hostname to query MX records for
--- @param servers  The list of servers to query, if empty use the default servers
--- @param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever
---
--- @return List of MX records, an empty list is returned if there are no MX records for the hostname
---@overload fun(hostname: string, servers: sf.IpAddress[]): sf.Dns.MxRecord[]
---@overload fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.Dns.MxRecord[]
---@param hostname string
---@return sf.Dns.MxRecord[]
function sf.Dns.queryMx(hostname) end
--- @brief Query SRV records for a hostname
---
--- @param hostname Hostname to query SRV records for
--- @param servers  The list of servers to query, if empty use the default servers
--- @param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever
---
--- @return List of SRV records, an empty list is returned if there are no SRV records for the hostname
---@overload fun(hostname: string, servers: sf.IpAddress[]): sf.Dns.SrvRecord[]
---@overload fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): sf.Dns.SrvRecord[]
---@param hostname string
---@return sf.Dns.SrvRecord[]
function sf.Dns.querySrv(hostname) end
--- @brief Query TXT records for a hostname
---
--- @param hostname Hostname to query TXT records for
--- @param servers  The list of servers to query, if empty use the default servers
--- @param timeout  Query timeout if using a provided list of servers, `std::nullopt` to wait forever
---
--- @return List of TXT record string lists, an empty list is returned if there are no TXT records for the hostname
---@overload fun(hostname: string, servers: sf.IpAddress[]): string[][]
---@overload fun(hostname: string, servers: sf.IpAddress[], timeout: sf.Time|nil): string[][]
---@param hostname string
---@return string[][]
function sf.Dns.queryTxt(hostname) end
--- @brief Get the computer's public address via DNS
---
--- The public address is the address of the computer from the
--- point of view of the internet, i.e. something like 89.54.1.169
--- or 2600:1901:0:13e0::1 as opposed to a private or local address
--- like 192.168.1.56 or fe80::1234:5678:9abc.
--- It is necessary for communication with hosts outside of the
--- local network.
---
--- The only way to reliably get the public address is to send
--- data to a host on the internet and see what the origin
--- address is; as a consequence, this function depends on both
--- your network connection and the server, and may be very slow.
--- You should try to use it as little as possible. Because this
--- function depends on the network connection and on a distant
--- server, you can specify a time limit if you don't want your
--- program to get stuck waiting in case there is a problem; this
--- limit is deactivated by default.
---
--- This function makes use of DNS queries get the public address.
---
--- @param timeout Maximum time to wait, `std::nullopt` to wait forever
--- @param type    The type of public address to get
---
--- @return Public IP address of the computer on success, `std::nullopt` otherwise
---@overload fun(timeout: sf.Time|nil, type: sf.IpAddress.Type): sf.IpAddress|nil
---@overload fun(timeout: sf.Time|nil): sf.IpAddress|nil
---@return sf.IpAddress|nil
function sf.Dns.getPublicAddress() end
--- @brief A FTP client
---
--- @deprecated Use `sf::Sftp` if possible.
---@class sf.Ftp
sf.Ftp = sf.Ftp or {}
--- @brief Default constructor
---@type fun(): sf.Ftp
sf.Ftp.new = function() end
--- @brief Connect to the specified FTP server
---
--- The port has a default value of 21, which is the standard
--- port used by the FTP protocol. You shouldn't use a different
--- value, unless you really know what you do.
--- This function tries to connect to the server so it may take
--- a while to complete, especially if the server is not
--- reachable. To avoid blocking your application for too long,
--- you can use a timeout. The default value, `Time::Zero`, means that the
--- system timeout will be used (which is usually pretty long).
---
--- @param server  Name or address of the FTP server to connect to
--- @param port    Port used for the connection
--- @param timeout Maximum time to wait
---
--- @return Server response to the request
---
--- @see `disconnect`
---@overload fun(self: sf.Ftp, server: sf.IpAddress, port: integer): sf.Ftp.Response
---@overload fun(self: sf.Ftp, server: sf.IpAddress): sf.Ftp.Response
---@param self sf.Ftp
---@param server sf.IpAddress
---@param port integer
---@param timeout sf.Time
---@return sf.Ftp.Response
function sf.Ftp.connect(self, server, port, timeout) end
--- @brief Close the connection with the server
---
--- @return Server response to the request
---
--- @see `connect`
---@type fun(self: sf.Ftp): sf.Ftp.Response
sf.Ftp.disconnect = function() end
--- @brief Log in using a username and a password
---
--- Logging in is mandatory after connecting to the server.
--- Users that are not logged in cannot perform any operation.
---
--- @param name     User name
--- @param password Password
---
--- @return Server response to the request
---@overload fun(self: sf.Ftp): sf.Ftp.Response
---@param self sf.Ftp
---@param name string
---@param password string
---@return sf.Ftp.Response
function sf.Ftp.login(self, name, password) end
--- @brief Send a null command to keep the connection alive
---
--- This command is useful because the server may close the
--- connection automatically if no command is sent.
---
--- @return Server response to the request
---@type fun(self: sf.Ftp): sf.Ftp.Response
sf.Ftp.keepAlive = function() end
--- @brief Get the current working directory
---
--- The working directory is the root path for subsequent
--- operations involving directories and/or filenames.
---
--- @return Server response to the request
---
--- @see `getDirectoryListing`, `changeDirectory`, `parentDirectory`
---@type fun(self: sf.Ftp): sf.Ftp.DirectoryResponse
sf.Ftp.getWorkingDirectory = function() end
--- @brief Get the contents of the given directory
---
--- This function retrieves the sub-directories and files
--- contained in the given directory. It is not recursive.
--- The `directory` parameter is relative to the current
--- working directory.
---
--- @param directory Directory to list
---
--- @return Server response to the request
---
--- @see `getWorkingDirectory`, `changeDirectory`, `parentDirectory`
---@overload fun(self: sf.Ftp): sf.Ftp.ListingResponse
---@param self sf.Ftp
---@param directory string
---@return sf.Ftp.ListingResponse
function sf.Ftp.getDirectoryListing(self, directory) end
--- @brief Change the current working directory
---
--- The new directory must be relative to the current one.
---
--- @param directory New working directory
---
--- @return Server response to the request
---
--- @see `getWorkingDirectory`, `getDirectoryListing`, `parentDirectory`
---@type fun(self: sf.Ftp, directory: string): sf.Ftp.Response
sf.Ftp.changeDirectory = function() end
--- @brief Go to the parent directory of the current one
---
--- @return Server response to the request
---
--- @see `getWorkingDirectory`, `getDirectoryListing`, `changeDirectory`
---@type fun(self: sf.Ftp): sf.Ftp.Response
sf.Ftp.parentDirectory = function() end
--- @brief Create a new directory
---
--- The new directory is created as a child of the current
--- working directory.
---
--- @param name Name of the directory to create
---
--- @return Server response to the request
---
--- @see `deleteDirectory`
---@type fun(self: sf.Ftp, name: string): sf.Ftp.Response
sf.Ftp.createDirectory = function() end
--- @brief Remove an existing directory
---
--- The directory to remove must be relative to the
--- current working directory.
--- Use this function with caution, the directory will
--- be removed permanently!
---
--- @param name Name of the directory to remove
---
--- @return Server response to the request
---
--- @see `createDirectory`
---@type fun(self: sf.Ftp, name: string): sf.Ftp.Response
sf.Ftp.deleteDirectory = function() end
--- @brief Rename an existing file
---
--- The file names must be relative to the current working
--- directory.
---
--- @param file    File to rename
--- @param newName New name of the file
---
--- @return Server response to the request
---
--- @see `deleteFile`
---@type fun(self: sf.Ftp, file: string, newName: string): sf.Ftp.Response
sf.Ftp.renameFile = function() end
--- @brief Remove an existing file
---
--- The file name must be relative to the current working
--- directory.
--- Use this function with caution, the file will be
--- removed permanently!
---
--- @param name File to remove
---
--- @return Server response to the request
---
--- @see `renameFile`
---@type fun(self: sf.Ftp, name: string): sf.Ftp.Response
sf.Ftp.deleteFile = function() end
--- @brief Download a file from the server
---
--- The file name of the distant file is relative to the
--- current working directory of the server, and the local
--- destination path is relative to the current directory
--- of your application.
--- If a file with the same file name as the distant file
--- already exists in the local destination path, it will
--- be overwritten.
---
--- @param remoteFile File name of the distant file to download
--- @param localPath  The directory in which to put the file on the local computer
--- @param mode       Transfer mode
---
--- @return Server response to the request
---
--- @see `upload`
---@overload fun(self: sf.Ftp, remoteFile: string, localPath: string): sf.Ftp.Response
---@param self sf.Ftp
---@param remoteFile string
---@param localPath string
---@param mode sf.Ftp.TransferMode
---@return sf.Ftp.Response
function sf.Ftp.download(self, remoteFile, localPath, mode) end
--- @brief Upload a file to the server
---
--- The name of the local file is relative to the current
--- working directory of your application, and the
--- remote path is relative to the current directory of the
--- FTP server.
---
--- The append parameter controls whether the remote file is
--- appended to or overwritten if it already exists.
---
--- @param localFile  Path of the local file to upload
--- @param remotePath The directory in which to put the file on the server
--- @param mode       Transfer mode
--- @param append     Pass `true` to append to or `false` to overwrite the remote file if it already exists
---
--- @return Server response to the request
---
--- @see `download`
---@overload fun(self: sf.Ftp, localFile: string, remotePath: string, mode: sf.Ftp.TransferMode): sf.Ftp.Response
---@overload fun(self: sf.Ftp, localFile: string, remotePath: string): sf.Ftp.Response
---@param self sf.Ftp
---@param localFile string
---@param remotePath string
---@param mode sf.Ftp.TransferMode
---@param append boolean
---@return sf.Ftp.Response
function sf.Ftp.upload(self, localFile, remotePath, mode, append) end
--- @brief Send a command to the FTP server
---
--- While the most often used commands are provided as member
--- functions in the `sf::Ftp` class, this method can be used
--- to send any FTP command to the server. If the command
--- requires one or more parameters, they can be specified
--- in `parameter`. If the server returns information, you
--- can extract it from the response using `Response::getMessage()`.
---
--- @param command   Command to send
--- @param parameter Command parameter
---
--- @return Server response to the request
---@overload fun(self: sf.Ftp, command: string): sf.Ftp.Response
---@param self sf.Ftp
---@param command string
---@param parameter string
---@return sf.Ftp.Response
function sf.Ftp.sendCommand(self, command, parameter) end
--- @brief Enumeration of transfer modes
---@class sf.Ftp.TransferMode
--- Binary mode (file is transferred as a sequence of bytes)
---@field Binary sf.Ftp.TransferMode
--- Text mode using ASCII encoding
---@field Ascii sf.Ftp.TransferMode
--- Text mode using EBCDIC encoding
---@field Ebcdic sf.Ftp.TransferMode
sf.Ftp.TransferMode = sf.Ftp.TransferMode or {}
--- @brief FTP response
---@class sf.Ftp.Response
sf.Ftp.Response = sf.Ftp.Response or {}
--- @brief Default constructor
---
--- This constructor is used by the FTP client to build
--- the response.
---
--- @param code    Response status code
--- @param message Response message
---@overload fun(code: sf.Ftp.Response.Status): sf.Ftp.Response
---@overload fun(): sf.Ftp.Response
---@param code sf.Ftp.Response.Status
---@param message string
---@return sf.Ftp.Response
function sf.Ftp.Response.new(code, message) end
--- @brief Check if the status code means a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the status code is < 400.
---
--- @return `true` if the status is a success, `false` if it is a failure
---@type fun(self: sf.Ftp.Response): boolean
sf.Ftp.Response.isOk = function() end
--- @brief Get the status code of the response
---
--- @return Status code
---@type fun(self: sf.Ftp.Response): sf.Ftp.Response.Status
sf.Ftp.Response.getStatus = function() end
--- @brief Get the full message contained in the response
---
--- @return The response message
---@type fun(self: sf.Ftp.Response): string
sf.Ftp.Response.getMessage = function() end
--- @brief Status codes possibly returned by a FTP response
---@class sf.Ftp.Response.Status
--- Restart marker reply
---@field RestartMarkerReply sf.Ftp.Response.Status
--- Service ready in N minutes
---@field ServiceReadySoon sf.Ftp.Response.Status
--- Data connection already opened, transfer starting
---@field DataConnectionAlreadyOpened sf.Ftp.Response.Status
--- File status ok, about to open data connection
---@field OpeningDataConnection sf.Ftp.Response.Status
--- Command ok
---@field Ok sf.Ftp.Response.Status
--- Command not implemented
---@field PointlessCommand sf.Ftp.Response.Status
--- System status, or system help reply
---@field SystemStatus sf.Ftp.Response.Status
--- Directory status
---@field DirectoryStatus sf.Ftp.Response.Status
--- File status
---@field FileStatus sf.Ftp.Response.Status
--- Help message
---@field HelpMessage sf.Ftp.Response.Status
--- NAME system type, where NAME is an official system name from the list in the Assigned Numbers document
---@field SystemType sf.Ftp.Response.Status
--- Service ready for new user
---@field ServiceReady sf.Ftp.Response.Status
--- Service closing control connection
---@field ClosingConnection sf.Ftp.Response.Status
--- Data connection open, no transfer in progress
---@field DataConnectionOpened sf.Ftp.Response.Status
--- Closing data connection, requested file action successful
---@field ClosingDataConnection sf.Ftp.Response.Status
--- Entering passive mode
---@field EnteringPassiveMode sf.Ftp.Response.Status
--- User logged in, proceed. Logged out if appropriate
---@field LoggedIn sf.Ftp.Response.Status
--- Requested file action ok
---@field FileActionOk sf.Ftp.Response.Status
--- PATHNAME created
---@field DirectoryOk sf.Ftp.Response.Status
--- User name ok, need password
---@field NeedPassword sf.Ftp.Response.Status
--- Need account for login
---@field NeedAccountToLogIn sf.Ftp.Response.Status
--- Requested file action pending further information
---@field NeedInformation sf.Ftp.Response.Status
--- Service not available, closing control connection
---@field ServiceUnavailable sf.Ftp.Response.Status
--- Can't open data connection
---@field DataConnectionUnavailable sf.Ftp.Response.Status
--- Connection closed, transfer aborted
---@field TransferAborted sf.Ftp.Response.Status
--- Requested file action not taken
---@field FileActionAborted sf.Ftp.Response.Status
--- Requested action aborted, local error in processing
---@field LocalError sf.Ftp.Response.Status
--- Requested action not taken; insufficient storage space in system, file unavailable
---@field InsufficientStorageSpace sf.Ftp.Response.Status
--- Syntax error, command unrecognized
---@field CommandUnknown sf.Ftp.Response.Status
--- Syntax error in parameters or arguments
---@field ParametersUnknown sf.Ftp.Response.Status
--- Command not implemented
---@field CommandNotImplemented sf.Ftp.Response.Status
--- Bad sequence of commands
---@field BadCommandSequence sf.Ftp.Response.Status
--- Command not implemented for that parameter
---@field ParameterNotImplemented sf.Ftp.Response.Status
--- Not logged in
---@field NotLoggedIn sf.Ftp.Response.Status
--- Need account for storing files
---@field NeedAccountToStore sf.Ftp.Response.Status
--- Requested action not taken, file unavailable
---@field FileUnavailable sf.Ftp.Response.Status
--- Requested action aborted, page type unknown
---@field PageTypeUnknown sf.Ftp.Response.Status
--- Requested file action aborted, exceeded storage allocation
---@field NotEnoughMemory sf.Ftp.Response.Status
--- Requested action not taken, file name not allowed
---@field FilenameNotAllowed sf.Ftp.Response.Status
--- Not part of the FTP standard, generated by SFML when a received response cannot be parsed
---@field InvalidResponse sf.Ftp.Response.Status
--- Not part of the FTP standard, generated by SFML when the low-level socket connection with the server fails
---@field ConnectionFailed sf.Ftp.Response.Status
--- Not part of the FTP standard, generated by SFML when the low-level socket connection is unexpectedly closed
---@field ConnectionClosed sf.Ftp.Response.Status
--- Not part of the FTP standard, generated by SFML when a local file cannot be read or written
---@field InvalidFile sf.Ftp.Response.Status
sf.Ftp.Response.Status = sf.Ftp.Response.Status or {}
--- @brief Specialization of FTP response returning a directory
---@class sf.Ftp.DirectoryResponse : sf.Ftp.Response
sf.Ftp.DirectoryResponse = sf.Ftp.DirectoryResponse or {}
--- @brief Default constructor
---
--- @param response Source response
---@type fun(response: sf.Ftp.Response): sf.Ftp.DirectoryResponse
sf.Ftp.DirectoryResponse.new = function() end
--- @brief Check if the status code means a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the status code is < 400.
---
--- @return `true` if the status is a success, `false` if it is a failure
---@type fun(self: sf.Ftp.DirectoryResponse): boolean
sf.Ftp.DirectoryResponse.isOk = function() end
--- @brief Get the status code of the response
---
--- @return Status code
---@type fun(self: sf.Ftp.DirectoryResponse): sf.Ftp.Response.Status
sf.Ftp.DirectoryResponse.getStatus = function() end
--- @brief Get the full message contained in the response
---
--- @return The response message
---@type fun(self: sf.Ftp.DirectoryResponse): string
sf.Ftp.DirectoryResponse.getMessage = function() end
--- @brief Get the directory returned in the response
---
--- @return Directory name
---@type fun(self: sf.Ftp.DirectoryResponse): string
sf.Ftp.DirectoryResponse.getDirectory = function() end
--- @brief Specialization of FTP response returning a
--- file name listing
---@class sf.Ftp.ListingResponse : sf.Ftp.Response
sf.Ftp.ListingResponse = sf.Ftp.ListingResponse or {}
--- @brief Default constructor
---
--- @param response  Source response
--- @param data      Data containing the raw listing
---@type fun(response: sf.Ftp.Response, data: string): sf.Ftp.ListingResponse
sf.Ftp.ListingResponse.new = function() end
--- @brief Check if the status code means a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the status code is < 400.
---
--- @return `true` if the status is a success, `false` if it is a failure
---@type fun(self: sf.Ftp.ListingResponse): boolean
sf.Ftp.ListingResponse.isOk = function() end
--- @brief Get the status code of the response
---
--- @return Status code
---@type fun(self: sf.Ftp.ListingResponse): sf.Ftp.Response.Status
sf.Ftp.ListingResponse.getStatus = function() end
--- @brief Get the full message contained in the response
---
--- @return The response message
---@type fun(self: sf.Ftp.ListingResponse): string
sf.Ftp.ListingResponse.getMessage = function() end
--- @brief Return the array of directory/file names
---
--- @return Array containing the requested listing
---@type fun(self: sf.Ftp.ListingResponse): string[]
sf.Ftp.ListingResponse.getListing = function() end
--- @brief A HTTP client
---@class sf.Http
sf.Http = sf.Http or {}
--- @brief Construct the HTTP client with the target host
---
--- This is equivalent to calling `setHost(host, port)`.
--- The port has a default value of 0, which means that the
--- HTTP client will use the right port according to the
--- protocol used (80 for HTTP). You should leave it like
--- this unless you really need a port other than the
--- standard one, or use an unknown protocol.
---
--- @param host        Web server to connect to
--- @param port        Port to use for the connection
--- @param addressType Address type to use for the connection, `std::nullopt` to specify no preference
---@overload fun(host: string): sf.Http
---@overload fun(): sf.Http
---@overload fun(host: string, port: integer, addressType: sf.IpAddress.Type|nil): sf.Http
---@param host string
---@param port integer
---@return sf.Http
function sf.Http.new(host, port) end
--- @brief Set the target host
---
--- This function just stores the host address and port, it
--- doesn't actually connect to it until you send a request.
--- It does however try to resolve the address.
--- The port has a default value of 0, which means that the
--- HTTP client will use the right port according to the
--- protocol used (80 for HTTP). You should leave it like
--- this unless you really need a port other than the
--- standard one, or use an unknown protocol.
---
--- @param host        Web server to connect to
--- @param port        Port to use for the connection
--- @param addressType Address type to use for the connection, `std::nullopt` to specify no preference
---
--- @return `true` if the host has been resolved and is valid, `false` otherwise
---@overload fun(self: sf.Http, host: string): boolean
---@overload fun(self: sf.Http, host: string, port: integer, addressType: sf.IpAddress.Type|nil): boolean
---@param self sf.Http
---@param host string
---@param port integer
---@return boolean
function sf.Http.setHost(self, host, port) end
--- @brief Send a HTTP request and return the server's response.
---
--- You must have a valid host before sending a request (see `setHost`).
--- Any missing mandatory header field in the request will be added
--- with an appropriate value.
--- Warning: this function waits for the server's response and may
--- not return instantly; use a thread if you don't want to block your
--- application, or use a timeout to limit the time to wait. A value
--- of `Time::Zero` means that the client will use the system default timeout
--- (which is usually pretty long).
---
--- @param request      Request to send
--- @param timeout      Maximum time to wait
--- @param verifyServer Verify the server if using HTTPS
---
--- @return Server's response
---@overload fun(self: sf.Http, request: sf.Http.Request, timeout: sf.Time): sf.Http.Response
---@overload fun(self: sf.Http, request: sf.Http.Request): sf.Http.Response
---@param self sf.Http
---@param request sf.Http.Request
---@param timeout sf.Time
---@param verifyServer boolean
---@return sf.Http.Response
function sf.Http.sendRequest(self, request, timeout, verifyServer) end
--- @brief HTTP request
---@class sf.Http.Request
sf.Http.Request = sf.Http.Request or {}
--- @brief Default constructor
---
--- This constructor creates a GET request, with the root
--- URI ("/") and an empty body.
---
--- @param uri    Target URI
--- @param method Method to use for the request
--- @param body   Content of the request's body
---@overload fun(uri: string, method: sf.Http.Request.Method): sf.Http.Request
---@overload fun(uri: string): sf.Http.Request
---@overload fun(): sf.Http.Request
---@param uri string
---@param method sf.Http.Request.Method
---@param body string
---@return sf.Http.Request
function sf.Http.Request.new(uri, method, body) end
--- @brief Set the value of a field
---
--- The field is created if it doesn't exist. The name of
--- the field is case-insensitive.
--- By default, a request doesn't contain any field (but the
--- mandatory fields are added later by the HTTP client when
--- sending the request).
---
--- @param field Name of the field to set
--- @param value Value of the field
---@type fun(self: sf.Http.Request, field: string, value: string)
sf.Http.Request.setField = function() end
--- @brief Set the request method
---
--- See the Method enumeration for a complete list of all
--- the available methods.
--- The method is `Http::Request::Method::Get` by default.
---
--- @param method Method to use for the request
---@type fun(self: sf.Http.Request, method: sf.Http.Request.Method)
sf.Http.Request.setMethod = function() end
--- @brief Set the requested URI
---
--- The URI is the resource (usually a web page or a file)
--- that you want to get or post.
--- The URI is "/" (the root page) by default.
---
--- @param uri URI to request, relative to the host
---@type fun(self: sf.Http.Request, uri: string)
sf.Http.Request.setUri = function() end
--- @brief Set the HTTP version for the request
---
--- The HTTP version is 1.0 by default.
---
--- @param major Major HTTP version number
--- @param minor Minor HTTP version number
---@type fun(self: sf.Http.Request, major: integer, minor: integer)
sf.Http.Request.setHttpVersion = function() end
--- @brief Set the body of the request
---
--- The body of a request is optional and only makes sense
--- for POST requests. It is ignored for all other methods.
--- The body is empty by default.
---
--- @param body Content of the body
---@type fun(self: sf.Http.Request, body: string)
sf.Http.Request.setBody = function() end
--- @brief Enumerate the available HTTP methods for a request
---@class sf.Http.Request.Method
--- Request in get mode, standard method to retrieve a page
---@field Get sf.Http.Request.Method
--- Request in post mode, usually to send data to a page
---@field Post sf.Http.Request.Method
--- Request a page's header only
---@field Head sf.Http.Request.Method
--- Request in put mode, useful for a REST API
---@field Put sf.Http.Request.Method
--- Request in delete mode, useful for a REST API
---@field Delete sf.Http.Request.Method
sf.Http.Request.Method = sf.Http.Request.Method or {}
--- @brief HTTP response
---@class sf.Http.Response
sf.Http.Response = sf.Http.Response or {}
---@type fun(): sf.Http.Response
sf.Http.Response.new = function() end
--- @brief Get the value of a field
---
--- If the field `field` is not found in the response header,
--- the empty string is returned. This function uses
--- case-insensitive comparisons.
---
--- @param field Name of the field to get
---
--- @return Value of the field, or empty string if not found
---@type fun(self: sf.Http.Response, field: string): string
sf.Http.Response.getField = function() end
--- @brief Get the response status code
---
--- The status code should be the first thing to be checked
--- after receiving a response, it defines whether it is a
--- success, a failure or anything else (see the Status
--- enumeration).
---
--- @return Status code of the response
---@type fun(self: sf.Http.Response): sf.Http.Response.Status
sf.Http.Response.getStatus = function() end
--- @brief Get the major HTTP version number of the response
---
--- @return Major HTTP version number
---
--- @see `getMinorHttpVersion`
---@type fun(self: sf.Http.Response): integer
sf.Http.Response.getMajorHttpVersion = function() end
--- @brief Get the minor HTTP version number of the response
---
--- @return Minor HTTP version number
---
--- @see `getMajorHttpVersion`
---@type fun(self: sf.Http.Response): integer
sf.Http.Response.getMinorHttpVersion = function() end
--- @brief Get the body of the response
---
--- The body of a response may contain:
--- @li the requested page (for GET requests)
--- @li a response from the server (for POST requests)
--- @li nothing (for HEAD requests)
--- @li an error message (in case of an error)
---
--- @return The response body
---@type fun(self: sf.Http.Response): string
sf.Http.Response.getBody = function() end
--- @brief Enumerate all the valid status codes for a response
---@class sf.Http.Response.Status
--- Most common code returned when operation was successful
---@field Ok sf.Http.Response.Status
--- The resource has successfully been created
---@field Created sf.Http.Response.Status
--- The request has been accepted, but will be processed later by the server
---@field Accepted sf.Http.Response.Status
--- The server didn't send any data in return
---@field NoContent sf.Http.Response.Status
--- The server informs the client that it should clear the view (form) that caused the request to be sent
---@field ResetContent sf.Http.Response.Status
--- The server has sent a part of the resource, as a response to a partial GET request
---@field PartialContent sf.Http.Response.Status
--- The requested page can be accessed from several locations
---@field MultipleChoices sf.Http.Response.Status
--- The requested page has permanently moved to a new location
---@field MovedPermanently sf.Http.Response.Status
--- The requested page has temporarily moved to a new location
---@field MovedTemporarily sf.Http.Response.Status
--- For conditional requests, means the requested page hasn't changed and doesn't need to be refreshed
---@field NotModified sf.Http.Response.Status
--- The server couldn't understand the request (syntax error)
---@field BadRequest sf.Http.Response.Status
--- The requested page needs an authentication to be accessed
---@field Unauthorized sf.Http.Response.Status
--- The requested page cannot be accessed at all, even with authentication
---@field Forbidden sf.Http.Response.Status
--- The requested page doesn't exist
---@field NotFound sf.Http.Response.Status
--- The server can't satisfy the partial GET request (with a "Range" header field)
---@field RangeNotSatisfiable sf.Http.Response.Status
--- The server encountered an unexpected error
---@field InternalServerError sf.Http.Response.Status
--- The server doesn't implement a requested feature
---@field NotImplemented sf.Http.Response.Status
--- The gateway server has received an error from the source server
---@field BadGateway sf.Http.Response.Status
--- The server is temporarily unavailable (overloaded, in maintenance, ...)
---@field ServiceNotAvailable sf.Http.Response.Status
--- The gateway server couldn't receive a response from the source server
---@field GatewayTimeout sf.Http.Response.Status
--- The server doesn't support the requested HTTP version
---@field VersionNotSupported sf.Http.Response.Status
--- Response is not a valid HTTP one
---@field InvalidResponse sf.Http.Response.Status
--- Connection with server failed
---@field ConnectionFailed sf.Http.Response.Status
sf.Http.Response.Status = sf.Http.Response.Status or {}
--- @brief Utility class to build blocks of data to transfer
--- over the network
---@class sf.Packet
sf.Packet = sf.Packet or {}
--- @brief Default constructor
---
--- Creates an empty packet.
---@type fun(): sf.Packet
sf.Packet.new = function() end
--- @brief Append data to the end of the packet
---
--- @param data        Pointer to the sequence of bytes to append
--- @param sizeInBytes Number of bytes to append
---
--- @see `clear`
--- @see `getReadPosition`
---@type fun(self: sf.Packet, data: any)
sf.Packet.append = function() end
--- @brief Get the current reading position in the packet
---
--- The next read operation will read data from this position
---
--- @return The byte offset of the current read position
---
--- @see `append`
---@type fun(self: sf.Packet): integer
sf.Packet.getReadPosition = function() end
--- @brief Clear the packet
---
--- After calling Clear, the packet is empty.
---
--- @see `append`
---@type fun(self: sf.Packet)
sf.Packet.clear = function() end
--- @brief Get a pointer to the data contained in the packet
---
--- Warning: the returned pointer may become invalid after
--- you append data to the packet, therefore it should never
--- be stored.
--- The return pointer is a `nullptr` if the packet is empty.
---
--- @return Pointer to the data
---
--- @see `getDataSize`
---@type fun(self: sf.Packet): nil
sf.Packet.getData = function() end
--- @brief Get the size of the data contained in the packet
---
--- This function returns the number of bytes pointed to by
--- what `getData` returns.
---
--- @return Data size, in bytes
---
--- @see `getData`
---@type fun(self: sf.Packet): integer
sf.Packet.getDataSize = function() end
--- @brief Tell if the reading position has reached the
--- end of the packet
---
--- This function is useful to know if there is some data
--- left to be read, without actually reading it.
---
--- @return `true` if all data was read, `false` otherwise
---
--- @see `operator` bool
---@type fun(self: sf.Packet): boolean
sf.Packet.endOfPacket = function() end

---@class sf.Packet
---@operator shl(any): sf.Packet
---@type fun(self: sf.Packet, data: boolean): sf.Packet
sf.Packet.writeBool = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeInt8 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeUInt8 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeInt16 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeUInt16 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeInt32 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeUInt32 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeInt64 = function() end
---@type fun(self: sf.Packet, data: integer): sf.Packet
sf.Packet.writeUInt64 = function() end
---@type fun(self: sf.Packet, data: number): sf.Packet
sf.Packet.writeFloat = function() end
---@type fun(self: sf.Packet, data: number): sf.Packet
sf.Packet.writeDouble = function() end
---@type fun(self: sf.Packet, data: string): sf.Packet
sf.Packet.writeString = function() end
---@type fun(self: sf.Packet, data: string): sf.Packet
sf.Packet.writeWideString = function() end
---@type fun(self: sf.Packet, data: string): sf.Packet
sf.Packet.writeSfString = function() end

---@class sf.Packet
---@operator shr(string): any
---@type fun(self: sf.Packet): boolean
sf.Packet.readBool = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readInt8 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readUInt8 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readInt16 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readUInt16 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readInt32 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readUInt32 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readInt64 = function() end
---@type fun(self: sf.Packet): integer
sf.Packet.readUInt64 = function() end
---@type fun(self: sf.Packet): number
sf.Packet.readFloat = function() end
---@type fun(self: sf.Packet): number
sf.Packet.readDouble = function() end
---@type fun(self: sf.Packet): string
sf.Packet.readString = function() end
---@type fun(self: sf.Packet): string
sf.Packet.readWideString = function() end
---@type fun(self: sf.Packet): string
sf.Packet.readSfString = function() end
--- @brief An SSH File Transfer Protocol (SFTP) client
---@class sf.Sftp
sf.Sftp = sf.Sftp or {}
--- @brief Default constructor
---@type fun(): sf.Sftp
sf.Sftp.new = function() end
--- @brief Connect to the specified SFTP server
---
--- The port has a default value of 22, which is the standard
--- port used by the SFTP protocol.
--- This function tries to connect to the server so it may take
--- a while to complete, especially if the server is not
--- reachable. To avoid blocking your application for too long,
--- you can use a timeout. The default value, `Time::Zero`, means that the
--- system timeout will be used (which is usually pretty long).
---
--- @param server  Name or address of the SFTP server to connect to
--- @param port    Port used for the connection
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of the connection attempt
---
--- @see `disconnect`
---@overload fun(self: sf.Sftp, server: sf.IpAddress, port: integer): sf.Sftp.Result
---@overload fun(self: sf.Sftp, server: sf.IpAddress): sf.Sftp.Result
---@param self sf.Sftp
---@param server sf.IpAddress
---@param port integer
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.connect(self, server, port, timeout) end
--- @brief Disconnect the connection with the server
---
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of disconnecting the connection with the server
---
--- @see `connect`
---@overload fun(self: sf.Sftp): sf.Sftp.Result
---@param self sf.Sftp
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.disconnect(self, timeout) end
--- @brief Get SSH session information
---
--- After connecting to the server and before actually
--- logging in the SSH session information of the underlying
--- connection will be available.
---
--- The session information contains among other things the
--- public key identifying the remote host and the connection
--- parameters such as encryption and compression used. The
--- identifiers used follow the RFC 4253 specification.
---
--- If the session information is not available `std::nullopt`
--- will be returned.
---
--- Because SSH was developed as a parallel standard to
--- SSL/TLS and automatic host certificate verification wasn't
--- widespread at the time, relying on the user to check the
--- authenticity of the host key was the typical method used
--- to verify that they were connecting to the legitimate host,
--- assuming the private key of the remote host was not
--- compromised.
---
--- If connection security is a high priority, examining
--- the parameters and aborting the connection if any weak
--- algorithms are used is also possible.
---
--- @return SSH session information or `std::nullopt` if it is not available
---@type fun(self: sf.Sftp): sf.Sftp.SessionInfo|nil
sf.Sftp.getSessionInfo = function() end
--- @brief Log in using a public/private key pair
---
--- Logging in is mandatory after connecting to the server.
--- Users that are not logged in cannot perform any operation.
---
--- This overload allows logging into the SFTP server using
--- public key authentication.
---
--- The public and private key data should be provided in PEM
--- format. PEM encoded data can be easily recognized by their
--- `-----BEGIN ............-----` header and
--- `-----END ............-----` footer.
---
--- Even though it is technically possible to derive the
--- public key from the private key, due to backend
--- limitations, providing a pre-generated public key as well
--- is necessary for this function to be able to succeed.
---
--- If the private key is protected by a passphrase the
--- passphrase can be provided as a NULL terminated string.
--- If the private key is not protected by a passphrase the
--- passphrase should be set to the empty string.
---
--- @param name                 User name
--- @param publicKeyData        Public key data
--- @param publicKeyLength      Public key data length
--- @param privateKeyData       Private key data
--- @param privateKeyLength     Private key data length
--- @param privateKeyPassphrase Private key passphrase, NULL terminated
--- @param timeout              Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of attempting to log in to the server
---@overload fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer, privateKeyPassphrase: string): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, publicKeyData: string, publicKeyLength: integer, privateKeyData: string, privateKeyLength: integer): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string, privateKeyPassphrase: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string, privateKeyPassphrase: string): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, password: string, timeout: sf.TimeoutWithPredicate): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, publicKeyData: string, privateKeyData: string): sf.Sftp.Result
---@overload fun(self: sf.Sftp, name: string, password: string): sf.Sftp.Result
---@param self sf.Sftp
---@param name string
---@param publicKeyData string
---@param publicKeyLength integer
---@param privateKeyData string
---@param privateKeyLength integer
---@param privateKeyPassphrase string
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.login(self, name, publicKeyData, publicKeyLength, privateKeyData, privateKeyLength, privateKeyPassphrase, timeout) end
--- @brief Resolve a remote path into an absolute remote path
---
--- Paths can contain links and other reserved path identifiers
--- such as . and .. referring to the current directory and
--- parent directory respectively.
---
--- When determining the absolute path, which does not contain
--- links or . or .. is necessary, this function can be used.
---
--- Resolving "." will return the absolute path to the current
--- working directory of the user after logging in to the SFTP
--- server.
---
--- @param path    Path to convert into an absolute path
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of converting the path into an absolute path
---
--- @see `getWorkingDirectory`
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.PathResult
---@param self sf.Sftp
---@param path string
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.PathResult
function sf.Sftp.resolvePath(self, path, timeout) end
--- @brief Get the current working directory on the server
---
--- This is an alias for calling `resolvePath(".")`.
---
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of getting the current working directory
---
--- @see `resolvePath`
---@overload fun(self: sf.Sftp): sf.Sftp.PathResult
---@param self sf.Sftp
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.PathResult
function sf.Sftp.getWorkingDirectory(self, timeout) end
--- @brief Get the attributes of a remote file or directory
---
--- Depending on whether `path` refers to a file or directory,
--- the attributes can contain e.g. the type of file, the file
--- owner, group, file size, modification and access times.
---
--- If links are not to be followed, `followLinks` can be set
--- to `false`. In this case the attributes of the link itself
--- will be returned.
---
--- @param path        Path to the remote file or directory whose attributes to get
--- @param followLinks `true` to follow links, `false` to return attributes of the link itself
--- @param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of getting the attributes
---
--- @see `getDirectoryListing`
---@overload fun(self: sf.Sftp, path: string, followLinks: boolean): sf.Sftp.AttributesResult
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.AttributesResult
---@param self sf.Sftp
---@param path string
---@param followLinks boolean
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.AttributesResult
function sf.Sftp.getAttributes(self, path, followLinks, timeout) end
--- @brief Get the contents of the given directory
---
--- This function retrieves the sub-directories and files
--- contained in the given directory. It is not recursive.
---
--- @param path    Path of the directory whose contents to list
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of getting the contents of the given directory
---
--- @see `getAttributes`
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.ListingResult
---@param self sf.Sftp
---@param path string
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.ListingResult
function sf.Sftp.getDirectoryListing(self, path, timeout) end
--- @brief Create a new directory
---
--- The new directory is created as a child of the current
--- working directory.
---
--- The default permissions value is equivalent to `rwxr-xr-x`
--- or 0755 in octal notation.
---
--- @param path        Path of the directory to create
--- @param permissions Permissions of the directory to create
--- @param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of creating the directory
---
--- @see `deleteDirectory`, `rename`
---@overload fun(self: sf.Sftp, path: string, permissions: any): sf.Sftp.Result
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.Result
---@param self sf.Sftp
---@param path string
---@param permissions any
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.createDirectory(self, path, permissions, timeout) end
--- @brief Remove an existing directory
---
--- Use this function with caution, the directory will
--- be removed permanently!
---
--- @param path    Path of the directory to remove
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of removing the directory
---
--- @see `createDirectory`, `rename`
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.Result
---@param self sf.Sftp
---@param path string
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.deleteDirectory(self, path, timeout) end
--- @brief Rename an existing file or directory
---
--- In POSIX renaming and moving and synonymous. If you want
--- to move a file or directory from one place to another
--- you rename it from an old to a new path.
---
--- If a file exists at the specified new path, depending
--- on whether `ovewrite` is set to true, the rename operation
--- will overwrite it or not. If a directory is being moved,
--- the new path must either not point to non-existant
--- directory or a directory that is empty. Non-empty
--- directories cannot be overwritten by this operation.
---
--- @param oldPath   Old path to the file or directory
--- @param newPath   New path to the file or directory
--- @param overwrite Set to `true` to allow overwriting a file that exists at `newPath`
--- @param timeout   Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of the operation
---@overload fun(self: sf.Sftp, oldPath: string, newPath: string, overwrite: boolean): sf.Sftp.Result
---@overload fun(self: sf.Sftp, oldPath: string, newPath: string): sf.Sftp.Result
---@param self sf.Sftp
---@param oldPath string
---@param newPath string
---@param overwrite boolean
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.rename(self, oldPath, newPath, overwrite, timeout) end
--- @brief Remove an existing file
---
--- Use this function with caution, the file will be
--- removed permanently!
---
--- @param path    Path to the file to remove
--- @param timeout Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of removing the file
---
--- @see `rename`
---@overload fun(self: sf.Sftp, path: string): sf.Sftp.Result
---@param self sf.Sftp
---@param path string
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.deleteFile(self, path, timeout) end
--- @brief Download a file from the server
---
--- This function retrieves the data in the file at the
--- remote path.
---
--- The file data is transferred in sequential blocks. For
--- every block of data transferred, the provided callback
--- is called. The callback is passed a pointer to a data
--- block and the size of the data contained in the current
--- block. This size can change over time so it is important
--- to always check the size value to know how much data is
--- actually available. The callback should return `true` to
--- indicate to the `download` function that it should
--- continue to transfer data. If the data transfer should
--- be aborted earlier, `false` can be returned from the
--- callback.
---
--- The function returns once all the data in the remote file
--- has been transferred or an error occurs or the function
--- times out.
---
--- If reading from the remote file should not start at the
--- beginning of the file, you can specify an offset in
--- bytes at which reading should start.
---
--- @param remotePath Path of the remote file whose data to download
--- @param callback   Callback to be called for every available data block
--- @param offset     Byte offset into the remote file at which reading should start
--- @param timeout    Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of downloading the file
---
--- @see `upload`
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean, offset: integer): sf.Sftp.Result
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(data: string, size: integer): boolean): sf.Sftp.Result
---@param self sf.Sftp
---@param remotePath string
---@param callback fun(data: string, size: integer): boolean
---@param offset integer
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.download(self, remotePath, callback, offset, timeout) end
--- @brief Upload a file to the server
---
--- This function writes data into a file at the remote path.
---
--- The file data is transferred in sequential blocks. Every
--- time the function wants to send a new block of data the
--- provided callback is called. The callback is passed a
--- pointer to a data block and a reference to the size of
--- the data block. Data to be sent should be copied into
--- the data block using e.g. `std::memcpy` and the size value
--- set to the actual number of bytes copied into the data
--- block. The size of the block can change over time so it
--- is important to check the size value that is passed to
--- the callback to know how many bytes can actually be
--- copied into the data block. The callback should return
--- `true` to indicate to the `upload` function that it
--- should continue to transfer data. Once the data transfer
--- should be stopped e.g. because there is no more data left
--- to send, `false` can be returned from the callback.
---
--- The function returns once all the data has been sent or
--- an error occurs or the function times out.
---
--- If a file does not exist at the remote path yet, it will
--- be created with the provided permissions.
---
--- If a file already exists at the remote path, setting
--- `truncate` to `true` will truncate the existing file
--- i.e. delete all pre-existing data before starting to
--- write the new data into the file.
---
--- Setting `append` to `true` will append to a file if
--- it already exists.
---
--- If writing to the remote file should not start at the
--- beginning of the file, you can specify an offset in
--- bytes at which writing should start.
---
--- The default permissions value is equivalent to `rw-r--r--`
--- or 0644 in octal notation.
---
--- @param remotePath  Path of the remote file in which to upload the data
--- @param callback    Callback to be called for every available data block
--- @param permissions Permissions of the remote file if it has to be created
--- @param truncate    Set to `true` to truncate the remote file if it already exists
--- @param append      Set to `true` to append to the remote file if it already exists
--- @param offset      Byte offset into the remote file at which writing should start
--- @param timeout     Maximum time to wait, optionally a predicate can be provided for more fine-grained control
---
--- @return Result of uploading the file
---
--- @see `download`
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean, append: boolean, offset: integer): sf.Sftp.Result
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean, append: boolean): sf.Sftp.Result
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any, truncate: boolean): sf.Sftp.Result
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil, permissions: any): sf.Sftp.Result
---@overload fun(self: sf.Sftp, remotePath: string, callback: fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil): sf.Sftp.Result
---@param self sf.Sftp
---@param remotePath string
---@param callback fun(capacity: integer): string|integer[]|{keepGoing: boolean?, data: string|integer[]?}|boolean|nil
---@param permissions any
---@param truncate boolean
---@param append boolean
---@param offset integer
---@param timeout sf.TimeoutWithPredicate
---@return sf.Sftp.Result
function sf.Sftp.upload(self, remotePath, callback, permissions, truncate, append, offset, timeout) end
--- @brief SFTP result
---@class sf.Sftp.Result
sf.Sftp.Result = sf.Sftp.Result or {}
--- @brief Constructor
---
--- This constructor is used by the SFTP client to build
--- the result.
---
--- @param value   Result value
--- @param message Result message
---@overload fun(value: sf.Sftp.Result.Value): sf.Sftp.Result
---@param value sf.Sftp.Result.Value
---@param message string
---@return sf.Sftp.Result
function sf.Sftp.Result.new(value, message) end
--- @brief Check if the result is a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the result value is `Value::Success`.
---
--- @return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`
---@type fun(self: sf.Sftp.Result): boolean
sf.Sftp.Result.isOk = function() end
--- @brief Get the result value
---
--- @return The result value
---@type fun(self: sf.Sftp.Result): sf.Sftp.Result.Value
sf.Sftp.Result.getValue = function() end
--- @brief Get the result message
---
--- @return The result message
---@type fun(self: sf.Sftp.Result): string
sf.Sftp.Result.getMessage = function() end
--- @brief Result values
---@class sf.Sftp.Result.Value
--- Operation completed successfully
---@field Success sf.Sftp.Result.Value
--- The TCP socket has been disconnected
---@field Disconnected sf.Sftp.Result.Value
--- Operation timed out
---@field Timeout sf.Sftp.Result.Value
--- Connection refused
---@field Refused sf.Sftp.Result.Value
--- Generic error
---@field Error sf.Sftp.Result.Value
--- Error during banner receive
---@field BannerReceive sf.Sftp.Result.Value
--- Error during banner send
---@field BannerSend sf.Sftp.Result.Value
--- Invalid message authentication code
---@field InvalidMac sf.Sftp.Result.Value
--- Allocation failure
---@field AllocationFailure sf.Sftp.Result.Value
--- Error sending on socket
---@field SocketSend sf.Sftp.Result.Value
--- Key exchange failed
---@field KeyExchangeFailure sf.Sftp.Result.Value
--- Host key initialization failed
---@field HostKeyInitialization sf.Sftp.Result.Value
--- Host key signing failed
---@field HostKeySign sf.Sftp.Result.Value
--- Decryption failed
---@field DecryptError sf.Sftp.Result.Value
--- SSH protocol error
---@field ProtocolError sf.Sftp.Result.Value
--- Password expired
---@field PasswordExpired sf.Sftp.Result.Value
--- File error
---@field FileError sf.Sftp.Result.Value
--- No method found
---@field MethodNone sf.Sftp.Result.Value
--- Authentication failed
---@field AuthenticationFailed sf.Sftp.Result.Value
--- Public key unverified
---@field PublicKeyUnverified sf.Sftp.Result.Value
--- Channel out of order
---@field ChannelOutOfOrder sf.Sftp.Result.Value
--- Channel failure
---@field ChannelFailure sf.Sftp.Result.Value
--- Channel request denied
---@field ChannelRequestDenied sf.Sftp.Result.Value
--- Channel unknown
---@field ChannelUnknown sf.Sftp.Result.Value
--- Channel window exceeded
---@field ChannelWindowExceeded sf.Sftp.Result.Value
--- Channel packet exceeded
---@field ChannelPacketExceeded sf.Sftp.Result.Value
--- Channel closed
---@field ChannelClosed sf.Sftp.Result.Value
--- Channel EOF sent
---@field ChannelEofSent sf.Sftp.Result.Value
--- SCP protocol error
---@field ScpProtocol sf.Sftp.Result.Value
--- Zlib error
---@field ZlibError sf.Sftp.Result.Value
--- Request denied
---@field RequestDenied sf.Sftp.Result.Value
--- Method not supported
---@field MethodNotSupported sf.Sftp.Result.Value
--- Invalid data
---@field InvalidData sf.Sftp.Result.Value
--- Public key protocol error
---@field PublicKeyProtocol sf.Sftp.Result.Value
--- Buffer too small
---@field BufferTooSmall sf.Sftp.Result.Value
--- Bad usage
---@field BadUse sf.Sftp.Result.Value
--- Compression error
---@field CompressError sf.Sftp.Result.Value
--- Out of boundary
---@field OutOfBoundary sf.Sftp.Result.Value
--- Agent protocol error
---@field AgentProtocol sf.Sftp.Result.Value
--- Socket receive error
---@field SocketRecv sf.Sftp.Result.Value
--- Encryption failed
---@field EncryptError sf.Sftp.Result.Value
--- Bad socket
---@field BadSocket sf.Sftp.Result.Value
--- Known hosts error
---@field KnownHosts sf.Sftp.Result.Value
--- Channel window full
---@field ChannelWindowFull sf.Sftp.Result.Value
--- Key file authentication failed
---@field KeyFileAuthenticationFailed sf.Sftp.Result.Value
--- End of file
---@field EndOfFile sf.Sftp.Result.Value
--- No such file
---@field NoSuchFile sf.Sftp.Result.Value
--- Permission denied
---@field PermissionDenied sf.Sftp.Result.Value
--- Failure
---@field Failure sf.Sftp.Result.Value
--- Bad message
---@field BadMessage sf.Sftp.Result.Value
--- No connection
---@field NoConnection sf.Sftp.Result.Value
--- Connection lost
---@field ConnectionLost sf.Sftp.Result.Value
--- Operation unsupported
---@field OperationUnsupported sf.Sftp.Result.Value
--- Invalid handle
---@field InvalidHandle sf.Sftp.Result.Value
--- No such path
---@field NoSuchPath sf.Sftp.Result.Value
--- File already exists
---@field FileAlreadyExists sf.Sftp.Result.Value
--- Write protect
---@field WriteProtect sf.Sftp.Result.Value
--- No media
---@field NoMedia sf.Sftp.Result.Value
--- No space on filesystem
---@field NoSpaceOnFileSystem sf.Sftp.Result.Value
--- Quota exceeded
---@field QuotaExceeded sf.Sftp.Result.Value
--- Unknown principal
---@field UnknownPrincipal sf.Sftp.Result.Value
--- Lock conflict
---@field LockConflict sf.Sftp.Result.Value
--- Directory not empty
---@field DirectoryNotEmpty sf.Sftp.Result.Value
--- Not a directory
---@field NotADirectory sf.Sftp.Result.Value
--- Invalid filename
---@field InvalidFilename sf.Sftp.Result.Value
--- Link loop
---@field LinkLoop sf.Sftp.Result.Value
--- Generic SFTP error
---@field SftpError sf.Sftp.Result.Value
sf.Sftp.Result.Value = sf.Sftp.Result.Value or {}
--- @brief Result of an operation returning a path
---@class sf.Sftp.PathResult : sf.Sftp.Result
sf.Sftp.PathResult = sf.Sftp.PathResult or {}
--- @brief Constructor
---
--- @param result Result
--- @param path   Path
---@type fun(result: sf.Sftp.Result, path: string): sf.Sftp.PathResult
sf.Sftp.PathResult.new = function() end
--- @brief Check if the result is a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the result value is `Value::Success`.
---
--- @return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`
---@type fun(self: sf.Sftp.PathResult): boolean
sf.Sftp.PathResult.isOk = function() end
--- @brief Get the result value
---
--- @return The result value
---@type fun(self: sf.Sftp.PathResult): sf.Sftp.Result.Value
sf.Sftp.PathResult.getValue = function() end
--- @brief Get the result message
---
--- @return The result message
---@type fun(self: sf.Sftp.PathResult): string
sf.Sftp.PathResult.getMessage = function() end
--- @brief Get the path
---
--- @return The path
---@type fun(self: sf.Sftp.PathResult): string
sf.Sftp.PathResult.getPath = function() end
--- @brief File or directory attributes
---@class sf.Sftp.Attributes
--- Path to the entry
---@field path string
--- Type of the entry
---@field type any|nil
--- Size of the entry
---@field size integer|nil
--- Permissions
---@field permissions any|nil
--- Owner user ID
---@field userId integer|nil
--- Group ID
---@field groupId integer|nil
--- Last access time
---@field accessTime any|nil
--- Last modification time
---@field modificationTime any|nil
sf.Sftp.Attributes = sf.Sftp.Attributes or {}
---@type fun(): sf.Sftp.Attributes
sf.Sftp.Attributes.new = function() end
--- @brief Result of an operation returning attributes
---@class sf.Sftp.AttributesResult : sf.Sftp.Result
sf.Sftp.AttributesResult = sf.Sftp.AttributesResult or {}
--- @brief Constructor
---
--- @param result     Result
--- @param attributes Attributes
---@type fun(result: sf.Sftp.Result, attributes: sf.Sftp.Attributes): sf.Sftp.AttributesResult
sf.Sftp.AttributesResult.new = function() end
--- @brief Check if the result is a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the result value is `Value::Success`.
---
--- @return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`
---@type fun(self: sf.Sftp.AttributesResult): boolean
sf.Sftp.AttributesResult.isOk = function() end
--- @brief Get the result value
---
--- @return The result value
---@type fun(self: sf.Sftp.AttributesResult): sf.Sftp.Result.Value
sf.Sftp.AttributesResult.getValue = function() end
--- @brief Get the result message
---
--- @return The result message
---@type fun(self: sf.Sftp.AttributesResult): string
sf.Sftp.AttributesResult.getMessage = function() end
--- @brief Get the attributes
---
--- @return The attributes
---@type fun(self: sf.Sftp.AttributesResult): sf.Sftp.Attributes
sf.Sftp.AttributesResult.getAttributes = function() end
--- @brief Result of an operation returning a directory listing
---@class sf.Sftp.ListingResult : sf.Sftp.Result
sf.Sftp.ListingResult = sf.Sftp.ListingResult or {}
--- @brief Constructor
---
--- @param result  Result
--- @param listing Directory listing
---@type fun(result: sf.Sftp.Result, listing: sf.Sftp.Attributes[]): sf.Sftp.ListingResult
sf.Sftp.ListingResult.new = function() end
--- @brief Check if the result is a success
---
--- This function is defined for convenience, it is
--- equivalent to testing if the result value is `Value::Success`.
---
--- @return `true` if the result is `Value::Success`, `false` if it is not `Value::Success`
---@type fun(self: sf.Sftp.ListingResult): boolean
sf.Sftp.ListingResult.isOk = function() end
--- @brief Get the result value
---
--- @return The result value
---@type fun(self: sf.Sftp.ListingResult): sf.Sftp.Result.Value
sf.Sftp.ListingResult.getValue = function() end
--- @brief Get the result message
---
--- @return The result message
---@type fun(self: sf.Sftp.ListingResult): string
sf.Sftp.ListingResult.getMessage = function() end
--- @brief Get the directory listing
---
--- @return The directory listing
---@type fun(self: sf.Sftp.ListingResult): sf.Sftp.Attributes[]
sf.Sftp.ListingResult.getListing = function() end
--- @brief Structure containing information about an active SFTP session
---@class sf.Sftp.SessionInfo
--- Host key
---@field hostKey sf.Sftp.SessionInfo.HostKey
--- Key exchange algorithm used in the session (RFC 4253)
---@field keyExchangeAlgorithm string
--- Host key algorithm used in the session (RFC 4253)
---@field hostKeyAlgorithm string
--- Client to server encryption algorithm used in the session (RFC 4253)
---@field clientToServerEncryptionAlgorithm string
--- Server to client encryption algorithm used in the session (RFC 4253)
---@field serverToClientEncryptionAlgorithm string
--- Client to server message authentication code algorithm used in the session (RFC 4253)
---@field clientToServerMacAlgorithm string
--- Server to client message authentication code algorithm used in the session (RFC 4253)
---@field serverToClientMacAlgorithm string
--- Client to server compression algorithm used in the session (RFC 4253)
---@field clientToServerCompressionAlgorithm string
--- Server to client compression algorithm used in the session (RFC 4253)
---@field serverToClientCompressionAlgorithm string
sf.Sftp.SessionInfo = sf.Sftp.SessionInfo or {}
---@type fun(): sf.Sftp.SessionInfo
sf.Sftp.SessionInfo.new = function() end
--- @brief Host key used to identify a host
---@class sf.Sftp.SessionInfo.HostKey
--- Host key type
---@field type sf.Sftp.SessionInfo.HostKey.Type
--- Host key data
---@field data any[]
--- Host key SHA1 hash
---@field sha1 any
--- Host key SHA256 hash
---@field sha256 any
sf.Sftp.SessionInfo.HostKey = sf.Sftp.SessionInfo.HostKey or {}
---@type fun(): sf.Sftp.SessionInfo.HostKey
sf.Sftp.SessionInfo.HostKey.new = function() end

---@class sf.Sftp.SessionInfo.HostKey.Type
--- Unknown key type
---@field Unknown sf.Sftp.SessionInfo.HostKey.Type
--- RSA
---@field Rsa sf.Sftp.SessionInfo.HostKey.Type
--- DSA
---@field Dsa sf.Sftp.SessionInfo.HostKey.Type
--- NIST P-256 ECDSA
---@field Ecdsa256 sf.Sftp.SessionInfo.HostKey.Type
--- NIST P-384 ECDSA
---@field Ecdsa384 sf.Sftp.SessionInfo.HostKey.Type
--- NIST P-521 ECDSA
---@field Ecdsa521 sf.Sftp.SessionInfo.HostKey.Type
--- ED25519
---@field Ed25519 sf.Sftp.SessionInfo.HostKey.Type
sf.Sftp.SessionInfo.HostKey.Type = sf.Sftp.SessionInfo.HostKey.Type or {}
--- @brief Base class for all the socket types
---@class sf.Socket
sf.Socket = sf.Socket or {}
--- @brief Set the blocking state of the socket
---
--- In blocking mode, calls will not return until they have
--- completed their task. For example, a call to Receive in
--- blocking mode won't return until some data was actually
--- received.
--- In non-blocking mode, calls will always return immediately,
--- using the return code to signal whether there was data
--- available or not.
--- By default, all sockets are blocking.
---
--- @param blocking `true` to set the socket as blocking, `false` for non-blocking
---
--- @see `isBlocking`
---@type fun(self: sf.Socket, blocking: boolean)
sf.Socket.setBlocking = function() end
--- @brief Tell whether the socket is in blocking or non-blocking mode
---
--- @return `true` if the socket is blocking, `false` otherwise
---
--- @see `setBlocking`
---@type fun(self: sf.Socket): boolean
sf.Socket.isBlocking = function() end
--- @brief Status codes that may be returned by socket functions
---@class sf.Socket.Status
--- The socket has sent / received the data
---@field Done sf.Socket.Status
--- The socket is not ready to send / receive data yet
---@field NotReady sf.Socket.Status
--- The socket sent a part of the data
---@field Partial sf.Socket.Status
--- The TCP socket has been disconnected
---@field Disconnected sf.Socket.Status
--- An unexpected error happened
---@field Error sf.Socket.Status
sf.Socket.Status = sf.Socket.Status or {}
--- Special value that tells the system to pick any available port
---@type integer
sf.Socket.AnyPort = nil
--- @brief Multiplexer that allows to read from multiple sockets
---@class sf.SocketSelector
sf.SocketSelector = sf.SocketSelector or {}
--- @brief Default constructor
---@type fun(): sf.SocketSelector
sf.SocketSelector.new = function() end
--- @brief Add a new socket to the selector
---
--- The type of readiness to wait for can be specified.
--- Specifying `SocketSelector::Receive` will wait for the
--- socket to become ready to receive data from, specifying
--- `SocketSelector::Send` will wait for the socket to become
--- ready to send data on. Specifying
--- `SocketSelector::Receive | SocketSelector::Send`
--- will wait for the socket to become either ready to send
--- or receive data on.
---
--- Adding a socket after it has already been added will just
--- overwrite the existing readiness type with the new value.
---
--- This function keeps a weak reference to the socket,
--- so you have to make sure that the socket is not destroyed
--- while it is stored in the selector.
--- This function does nothing if the socket is not valid.
---
--- When adding a socket to the selector you can also attach
--- a callback along with it. The callback is called by
--- `dispatchReadyCallbacks` when a socket is determined to be
--- ready after a call to `wait`.
---
--- Using attached callbacks instead of having to individually
--- call `isReady` on every socket after every call to `wait`
--- allows for scaling up to a large number of sockets. This
--- is because the overhead of checking for socket readiness
--- using `isReady` grows proportionally to the total number
--- of sockets. When using callbacks calling `isReady` on
--- every socket is no longer necessary.
---
--- Because a socket can be ready for receiving, sending or
--- both, the type of readiness is passed to the attached
--- callback as a bitwise combination of
--- `SocketSelector::Receive` and/or `SocketSelector::Send`
--- when it is called by `dispatchReadyCallbacks`.
--- Some systems don't support combined read and write
--- notifications. On these systems, if a socket is ready
--- to be both received from and sent to the callback will be
--- called twice, once with `SocketSelector::Receive` and once
--- with `SocketSelector::Send`.
---
--- To remove the attached callback of a socket, call `add`
--- again with an empty function.
---
--- By default, no readiness callback is attached when adding
--- a socket.
---
--- @param socket        Reference to the socket to add
--- @param readinessType Type of readiness to wait for, a bitwise combination of `SocketSelector::Receive` and/or `SocketSelector::Send`
--- @param readyCallback Ready callback to attach to the socket, pass an empty function to remove the ready callback
---
--- @return `true` if the socket was added successfully, `false` otherwise
---
--- @see `remove`, `clear`
---@overload fun(self: sf.SocketSelector, socket: sf.Socket): boolean
---@overload fun(self: sf.SocketSelector, socket: sf.Socket, readinessType: integer, readyCallback: fun(arg1: sf.SocketSelector.ReadinessType)): boolean
---@param self sf.SocketSelector
---@param socket sf.Socket
---@param readinessType integer
---@return boolean
function sf.SocketSelector.add(self, socket, readinessType) end
--- @brief Remove a socket from the selector
---
--- This function doesn't destroy the socket, it simply
--- removes the reference that the selector has to it.
---
--- @param socket Reference to the socket to remove
---
--- @return `true` if the socket was removed successfully, `false` otherwise
---
--- @see `add`, `clear`
---@type fun(self: sf.SocketSelector, socket: sf.Socket): boolean
sf.SocketSelector.remove = function() end
--- @brief Remove all the sockets stored in the selector
---
--- This function doesn't destroy any instance, it simply
--- removes all the references that the selector has to
--- external sockets.
---
--- @see `add`, `remove`
---@type fun(self: sf.SocketSelector)
sf.SocketSelector.clear = function() end
--- @brief Wait until one or more sockets are ready to receive or send
---
--- This function returns as soon as at least one socket has
--- some data available to be received or data can be sent,
--- depending on how the socket was added to this selector.
--- To know which sockets are ready, use the `isReady` function.
--- If you use a timeout and no socket is ready before the timeout
--- is over, the function returns `false`.
---
--- @param timeout Maximum time to wait, (use Time::Zero for infinity)
---
--- @return `true` if there are sockets ready, `false` otherwise
---
--- @see `isReady`, `dispatchReadyCallbacks`
---@overload fun(self: sf.SocketSelector): boolean
---@param self sf.SocketSelector
---@param timeout sf.Time
---@return boolean
function sf.SocketSelector.wait(self, timeout) end
--- @brief Test a socket to know if it is ready to receive or send data
---
--- This function must be used after a call to `wait`, to know
--- which sockets are ready to receive or send data. If a socket
--- is ready, a call to receive or send will never block because
--- we know that there is data available to read or we can write.
--- Note that if this function returns `true` for a TcpListener,
--- this means that it is ready to accept a new connection.
---
--- @param socket        Socket to test
--- @param readinessType Type of readiness to check for, a bitwise combination of `SocketSelector::Receive` and/or `SocketSelector::Send`
---
--- @return `true` if the socket is ready to read, `false` otherwise
---
--- @see `wait`
---@overload fun(self: sf.SocketSelector, socket: sf.Socket): boolean
---@param self sf.SocketSelector
---@param socket sf.Socket
---@param readinessType integer
---@return boolean
function sf.SocketSelector.isReady(self, socket, readinessType) end
--- @brief Dispatch callbacks of ready sockets
---
--- After calling `wait` returns `true`, at least one socket
--- is ready to receive or send data. Calling
--- `dispatchReadyCallbacks` will call the attached ready
--- callback for every socket that is ready to either receive
--- or send data. Sockets that don't have a callback attached
--- can still be individually checked using `isReady`.
---
--- The readiness state of each socket is maintained until
--- the next call to `wait`. Calling `dispatchReadyCallbacks`
--- multiple times after a single call to `wait` will run the
--- exact same callbacks with the exact same passed arguments.
---
--- @see `wait`
---@type fun(self: sf.SocketSelector)
sf.SocketSelector.dispatchReadyCallbacks = function() end
---@alias sf.SocketSelector.ReadinessType sf.SocketSelector.unsigned int
--- Check if sockets are ready to be received from
---@type sf.SocketSelector.ReadinessType
sf.SocketSelector.Receive = nil
--- Check if sockets are ready to be sent to
---@type sf.SocketSelector.ReadinessType
sf.SocketSelector.Send = nil
--- @brief Socket that listens to new TCP connections
---@class sf.TcpListener : sf.Socket
sf.TcpListener = sf.TcpListener or {}
--- @brief Default constructor
---@type fun(): sf.TcpListener
sf.TcpListener.new = function() end
--- @brief Set the blocking state of the socket
---
--- In blocking mode, calls will not return until they have
--- completed their task. For example, a call to Receive in
--- blocking mode won't return until some data was actually
--- received.
--- In non-blocking mode, calls will always return immediately,
--- using the return code to signal whether there was data
--- available or not.
--- By default, all sockets are blocking.
---
--- @param blocking `true` to set the socket as blocking, `false` for non-blocking
---
--- @see `isBlocking`
---@type fun(self: sf.TcpListener, blocking: boolean)
sf.TcpListener.setBlocking = function() end
--- @brief Tell whether the socket is in blocking or non-blocking mode
---
--- @return `true` if the socket is blocking, `false` otherwise
---
--- @see `setBlocking`
---@type fun(self: sf.TcpListener): boolean
sf.TcpListener.isBlocking = function() end
--- @brief Get the port to which the socket is bound locally
---
--- If the socket is not listening to a port, this function
--- returns 0.
---
--- @return Port to which the socket is bound
---
--- @see `listen`
---@type fun(self: sf.TcpListener): integer
sf.TcpListener.getLocalPort = function() end
--- @brief Start listening for incoming connection attempts
---
--- This function makes the socket start listening on the
--- specified port, waiting for incoming connection attempts.
---
--- If the socket is already listening on a port when this
--- function is called, it will stop listening on the old
--- port before starting to listen on the new port.
---
--- When providing `sf::Socket::AnyPort` as port, the listener
--- will request an available port from the system.
--- The chosen port can be retrieved by calling `getLocalPort()`.
---
--- @param port    Port to listen on for incoming connection attempts
--- @param address Address of the interface to listen on
---
--- @return Status code
---
--- @see `accept`, `close`
---@overload fun(self: sf.TcpListener, port: integer): sf.Socket.Status
---@param self sf.TcpListener
---@param port integer
---@param address sf.IpAddress
---@return sf.Socket.Status
function sf.TcpListener.listen(self, port, address) end
--- @brief Stop listening and close the socket
---
--- This function gracefully stops the listener. If the
--- socket is not listening, this function has no effect.
---
--- @see `listen`
---@type fun(self: sf.TcpListener)
sf.TcpListener.close = function() end
--- @brief Accept a new connection
---
--- If the socket is in blocking mode, this function will
--- not return until a connection is actually received.
---
--- @param socket Socket that will hold the new connection
---
--- @return Status code
---
--- @see `listen`
---@type fun(self: sf.TcpListener): sf.Socket.Status, any
sf.TcpListener.accept = function() end
--- @brief Specialized socket using the TCP protocol
---@class sf.TcpSocket : sf.Socket
sf.TcpSocket = sf.TcpSocket or {}
--- @brief Default constructor
---@type fun(): sf.TcpSocket
sf.TcpSocket.new = function() end
--- @brief Set the blocking state of the socket
---
--- In blocking mode, calls will not return until they have
--- completed their task. For example, a call to Receive in
--- blocking mode won't return until some data was actually
--- received.
--- In non-blocking mode, calls will always return immediately,
--- using the return code to signal whether there was data
--- available or not.
--- By default, all sockets are blocking.
---
--- @param blocking `true` to set the socket as blocking, `false` for non-blocking
---
--- @see `isBlocking`
---@type fun(self: sf.TcpSocket, blocking: boolean)
sf.TcpSocket.setBlocking = function() end
--- @brief Tell whether the socket is in blocking or non-blocking mode
---
--- @return `true` if the socket is blocking, `false` otherwise
---
--- @see `setBlocking`
---@type fun(self: sf.TcpSocket): boolean
sf.TcpSocket.isBlocking = function() end
--- @brief Get the port to which the socket is bound locally
---
--- If the socket is not connected, this function returns 0.
---
--- @return Port to which the socket is bound
---
--- @see `connect`, `getRemotePort`
---@type fun(self: sf.TcpSocket): integer
sf.TcpSocket.getLocalPort = function() end
--- @brief Get the address of the connected peer
---
--- If the socket is not connected, this function returns
--- an unset optional.
---
--- @return Address of the remote peer
---
--- @see `getRemotePort`
---@type fun(self: sf.TcpSocket): sf.IpAddress|nil
sf.TcpSocket.getRemoteAddress = function() end
--- @brief Get the port of the connected peer to which
--- the socket is connected
---
--- If the socket is not connected, this function returns 0.
---
--- @return Remote port to which the socket is connected
---
--- @see `getRemoteAddress`
---@type fun(self: sf.TcpSocket): integer
sf.TcpSocket.getRemotePort = function() end
--- @brief Connect the socket to a remote peer
---
--- In blocking mode, this function may take a while, especially
--- if the remote peer is not reachable. The last parameter allows
--- you to stop trying to connect after a given timeout.
--- If the socket is already connected, the connection is
--- forcibly disconnected before attempting to connect again.
---
--- @param remoteAddress Address of the remote peer
--- @param remotePort    Port of the remote peer
--- @param timeout       Optional maximum time to wait
---
--- @return Status code
---
--- @see `disconnect`
---@overload fun(self: sf.TcpSocket, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status
---@param self sf.TcpSocket
---@param remoteAddress sf.IpAddress
---@param remotePort integer
---@param timeout sf.Time
---@return sf.Socket.Status
function sf.TcpSocket.connect(self, remoteAddress, remotePort, timeout) end
--- @brief Disconnect the socket from its remote peer
---
--- This function gracefully closes the connection. If the
--- socket is not connected, this function has no effect.
---
--- @see `connect`
---@type fun(self: sf.TcpSocket)
sf.TcpSocket.disconnect = function() end
--- @brief Set up transport layer security as a client
---
--- Once the TCP connection is connected, transport layer
--- security can be set up.
---
--- All the necessary cryptographic initialization will
--- be performed when this function is called.
---
--- If this function is called before the TCP connection is
--- connected, it will return `TlsStatus::NotConnected` and
--- must be called again once the TCP connection is connected.
---
--- If this function started TLS setup but could not finish
--- it within this call e.g. because this socket was set to
--- non-blocking, it will return `TlsStatus::HandshakeStarted`
--- and this function will have to be called repeatedly until
--- `TlsStatus::HandshakeComplete` is returned. If this socket
--- is blocking, `TlsStatus::HandshakeComplete` should be
--- returned within the same function call if TLS setup was
--- successful.
---
--- If `TlsStatus::Error` is returned, something went wrong
--- with TLS setup and the connection must be reconnected and
--- TLS setup reattempted after it is connected again.
---
--- If verification is enabled, this function verifies the peer
--- using the system provided certificate store. If the peer
--- does not have a certificate that was signed by a certificate
--- authority i.e. a self-signed certificate, the entire certificate
--- chain can be provided using the alternative overload.
---
--- Servers that host multiple services under different names
--- need to know which of those services we want to connect
--- to in order to reply with the correct certificate chain.
--- Server name indication (SNI) is used for this purpose. The
--- hostname provided to this function is sent to the server
--- if it supports SNI in order for it to return the corresponding
--- certificate chain. The hostname is then used to verify the
--- certificate chain that was returned by the server. If the
--- server does not support SNI or only serves a single
--- certificate chain, the hostname will only be used for
--- verification.
---
--- @param hostname   Hostname of the remote peer, used for verification
--- @param verifyPeer `true` to enable peer verification, `false` to disable it
---
--- @return TLS status code
---
--- @see `setupTlsServer`
---@overload fun(self: sf.TcpSocket, hostname: string, certificateChainData: string): sf.TcpSocket.TlsStatus
---@overload fun(self: sf.TcpSocket, hostname: string): sf.TcpSocket.TlsStatus
---@overload fun(self: sf.TcpSocket, hostname: string, certificateChainData: any): sf.TcpSocket.TlsStatus
---@param self sf.TcpSocket
---@param hostname string
---@param verifyPeer boolean
---@return sf.TcpSocket.TlsStatus
function sf.TcpSocket.setupTlsClient(self, hostname, verifyPeer) end
--- @brief Set up transport layer security as a server
---
--- Once the TCP connection is connected, transport layer
--- security can be set up.
---
--- All the necessary cryptographic initialization will
--- be performed when this function is called.
---
--- If this function is called before the TCP connection is
--- connected, it will return `TlsStatus::NotConnected` and
--- must be called again once the TCP connection is connected.
---
--- If this function started TLS setup but could not finish
--- it within this call e.g. because this socket was set to
--- non-blocking, it will return `TlsStatus::HandshakeStarted`
--- and this function will have to be called repeatedly until
--- `TlsStatus::HandshakeComplete` is returned. If this socket
--- is blocking, `TlsStatus::HandshakeComplete` should be
--- returned within the same function call if TLS setup was
--- successful.
---
--- If `TlsStatus::Error` is returned, something went wrong
--- with TLS setup and the connection must be disconnected.
--- The client must reconnect and reattempt TLS setup again.
---
--- As a server, a certificate chain as well as a private key
--- must be provided.
---
--- The certificate and private key data should be provided in
--- PEM format.
---
--- If the private key is secured by a password, the password
--- must be provided.
---
--- @param certificateChainData   Certificate chain data in PEM encoding
--- @param privateKeyData         Private key data in PEM encoding
--- @param privateKeyPasswordData Private key password if required
---
--- @return TLS status code
---
--- @see `setupTlsClient`
---@overload fun(self: sf.TcpSocket, certificateChainData: string, privateKeyData: string): sf.TcpSocket.TlsStatus
---@overload fun(self: sf.TcpSocket, certificateChainData: any, privateKeyData: any, privateKeyPasswordData: any): sf.TcpSocket.TlsStatus
---@param self sf.TcpSocket
---@param certificateChainData string
---@param privateKeyData string
---@param privateKeyPasswordData string
---@return sf.TcpSocket.TlsStatus
function sf.TcpSocket.setupTlsServer(self, certificateChainData, privateKeyData, privateKeyPasswordData) end
--- @brief Get the name of the TLS ciphersuite currently in use
---
--- @return TLS ciphersuite currently in use or `std::nullopt` if TLS is not set up
---
--- @see `setupTlsClient`, `setupTlsServer`
---@type fun(self: sf.TcpSocket): string|nil
sf.TcpSocket.getCurrentCiphersuiteName = function() end
--- @brief Send a formatted packet of data to the remote peer
---
--- In non-blocking mode, if this function returns `sf::Socket::Status::Partial`,
--- you @em must retry sending the same unmodified packet before sending
--- anything else in order to guarantee the packet arrives at the remote
--- peer uncorrupted.
--- This function will fail if the socket is not connected.
---
--- @param packet Packet to send
---
--- @return Status code
---
--- @see `receive`
---@overload fun(self: sf.TcpSocket, data: any): sf.Socket.Status, any
---@param self sf.TcpSocket
---@param packet sf.Packet
---@return sf.Socket.Status
function sf.TcpSocket.send(self, packet) end
--- @brief Receive raw data from the remote peer
---
--- In blocking mode, this function will wait until some
--- bytes are actually received.
--- This function will fail if the socket is not connected.
---
--- @param data     Pointer to the array to fill with the received bytes
--- @param size     Maximum number of bytes that can be received
--- @param received This variable is filled with the actual number of bytes received
---
--- @return Status code
---
--- @see `send`
---@overload fun(self: sf.TcpSocket): sf.Socket.Status, any
---@param self sf.TcpSocket
---@param size integer
---@return sf.Socket.Status
---@return any
---@return any
function sf.TcpSocket.receive(self, size) end
--- @brief TLS status codes that may be returned by TLS setup
---@class sf.TcpSocket.TlsStatus
--- TCP connection not yet connected
---@field NotConnected sf.TcpSocket.TlsStatus
--- TLS handshake has been started
---@field HandshakeStarted sf.TcpSocket.TlsStatus
--- TLS handshake is complete, stream is encrypted
---@field HandshakeComplete sf.TcpSocket.TlsStatus
--- An unexpected error happened
---@field Error sf.TcpSocket.TlsStatus
sf.TcpSocket.TlsStatus = sf.TcpSocket.TlsStatus or {}
--- @brief Specialized socket using the UDP protocol
---@class sf.UdpSocket : sf.Socket
sf.UdpSocket = sf.UdpSocket or {}
--- @brief Default constructor
---@type fun(): sf.UdpSocket
sf.UdpSocket.new = function() end
--- @brief Set the blocking state of the socket
---
--- In blocking mode, calls will not return until they have
--- completed their task. For example, a call to Receive in
--- blocking mode won't return until some data was actually
--- received.
--- In non-blocking mode, calls will always return immediately,
--- using the return code to signal whether there was data
--- available or not.
--- By default, all sockets are blocking.
---
--- @param blocking `true` to set the socket as blocking, `false` for non-blocking
---
--- @see `isBlocking`
---@type fun(self: sf.UdpSocket, blocking: boolean)
sf.UdpSocket.setBlocking = function() end
--- @brief Tell whether the socket is in blocking or non-blocking mode
---
--- @return `true` if the socket is blocking, `false` otherwise
---
--- @see `setBlocking`
---@type fun(self: sf.UdpSocket): boolean
sf.UdpSocket.isBlocking = function() end
--- @brief Get the port to which the socket is bound locally
---
--- If the socket is not bound to a port, this function
--- returns 0.
---
--- @return Port to which the socket is bound
---
--- @see `bind`
---@type fun(self: sf.UdpSocket): integer
sf.UdpSocket.getLocalPort = function() end
--- @brief Bind the socket to a specific port
---
--- Binding the socket to a port is necessary for being
--- able to receive data on that port.
---
--- When providing `sf::Socket::AnyPort` as port, the listener
--- will request an available port from the system.
--- The chosen port can be retrieved by calling `getLocalPort()`.
---
--- Since the socket can only be bound to a single port at
--- any given moment, if it is already bound when this
--- function is called, it will be unbound from the previous
--- port before being bound to the new one.
---
--- @param port    Port to bind the socket to
--- @param address Address of the interface to bind to
---
--- @return Status code
---
--- @see `unbind`, `getLocalPort`
---@overload fun(self: sf.UdpSocket, port: integer): sf.Socket.Status
---@param self sf.UdpSocket
---@param port integer
---@param address sf.IpAddress
---@return sf.Socket.Status
function sf.UdpSocket.bind(self, port, address) end
--- @brief Unbind the socket from the local port to which it is bound
---
--- The port that the socket was previously bound to is immediately
--- made available to the operating system after this function is called.
--- This means that a subsequent call to `bind()` will be able to re-bind
--- the port if no other process has done so in the mean time.
--- If the socket is not bound to a port, this function has no effect.
---
--- @see `bind`
---@type fun(self: sf.UdpSocket)
sf.UdpSocket.unbind = function() end
--- @brief Send a formatted packet of data to a remote peer
---
--- Make sure that the packet size is not greater than
--- `UdpSocket::MaxDatagramSize`, otherwise this function will
--- fail and no data will be sent.
---
--- @param packet        Packet to send
--- @param remoteAddress Address of the receiver
--- @param remotePort    Port of the receiver to send the data to
---
--- @return Status code
---
--- @see `receive`
---@overload fun(self: sf.UdpSocket, data: any, remoteAddress: sf.IpAddress, remotePort: integer): sf.Socket.Status
---@param self sf.UdpSocket
---@param packet sf.Packet
---@param remoteAddress sf.IpAddress
---@param remotePort integer
---@return sf.Socket.Status
function sf.UdpSocket.send(self, packet, remoteAddress, remotePort) end
--- @brief Receive raw data from a remote peer
---
--- In blocking mode, this function will wait until some
--- bytes are actually received.
--- Be careful to use a buffer which is large enough for
--- the data that you intend to receive, if it is too small
--- then an error will be returned and *all* the data will
--- be lost.
---
--- @param data          Pointer to the array to fill with the received bytes
--- @param size          Maximum number of bytes that can be received
--- @param received      This variable is filled with the actual number of bytes received
--- @param remoteAddress Address of the peer that sent the data
--- @param remotePort    Port of the peer that sent the data
---
--- @return Status code
---
--- @see `send`
---@overload fun(self: sf.UdpSocket): sf.Socket.Status, any, any, any
---@param self sf.UdpSocket
---@param size integer
---@return sf.Socket.Status
---@return any
---@return any
---@return any
---@return any
function sf.UdpSocket.receive(self, size) end
--- The maximum number of bytes that can be sent in a single UDP datagram
---@type integer
sf.UdpSocket.MaxDatagramSize = nil
