#ifndef _MOTION_H_
#define _MOTION_H_

/**
 * @enum deviceMotionStatus_t
 * @brief Enumerates the motion status of the device.
 */
typedef enum {
    DEVICE_ON_MOTION,    //!< The device is moving.
    DEVICE_STATIONARY,   //!< The device is stationary.
} deviceMotionStatus_t;

/**
 * @struct euler_t
 * @brief Stores orientation angles in degrees.
 */
struct euler_t {
  float yaw;    //!< Rotation around the vertical axis (Z).
  float pitch;  //!< Rotation around the lateral axis (Y).
  float roll;   //!< Rotation around the longitudinal axis (X).
};

/**
 * @struct acceleration_t
 * @brief Stores acceleration values on each axis in m/s².
 */
struct acceleration_t {
  float ax; //!< Acceleration in the X-axis.
  float ay; //!< Acceleration in the Y-axis.
  float az; //!< Acceleration in the Z-axis.
};

/**
 * @struct IMUData_t
 * @brief Holds sensor data including orientation, acceleration, and metadata.
 */
struct IMUData_t {
  euler_t angles;                 //!< Euler angles (yaw, pitch, roll).
  acceleration_t acceleration;   //!< Linear acceleration values.
  uint8_t status;                //!< Sensor status byte.
  char * timestamp;              //!< Timestamp of the measurement (optional).
  int timeBetweenSamples;        //!< Time elapsed between samples in ms.
};

#endif // _MOTION_H_