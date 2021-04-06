<p align="center">
  <img src="documentation/images/logo.png"/>
  
  <h3 align="center"> Wall-E 2.2 </h3>
  <p align="center">
    Code Repository for <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta">Wall-E 2.2</a>
    <br />
    <br />
    <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/issues">Report Bug</a>
    ·
    <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/issues">Request Feature</a>
    ·
    <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/pulls">Send a Pull Request</a>
  </p>
 </p>
 
 <p align="center">
  
  <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/network/members">
    <img src="https://img.shields.io/github/forks/SRA-VJTI/Wall-E_v2.2-beta">
  </a>
  <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/stargazers">
    <img src="https://img.shields.io/github/stars/SRA-VJTI/Wall-E_v2.2-beta">
  </a>
  <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/issues">
    <img src="https://img.shields.io/github/issues/SRA-VJTI/Wall-E_v2.2-beta">
  </a>
  <a href="https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/master/LICENSE">
    <img src="https://img.shields.io/github/license/SRA-VJTI/Wall-E_v2.2-beta">
  </a>
  <a href="https://linkedin.com/in/sra-vjti">
    <img src="https://img.shields.io/badge/-LinkedIn-black.svg?logo=linkedin&colorB=555">
  </a>
</p>

## Table of Contents

- [About](#about-the-workshop)
- [Installations](#installations)
- [Content](#content)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)
- [Acknowledgements](#acknowledgements)

## About the Workshop
<p align="center">
  <img src="documentation/images/wall_E_bot.JPG"/>
  <h3 align="center"> Wall-E bot Simulation </h3>
</p>
<p align="center">
  <img src="documentation/images/walle bot.png"/>
  <h3 align="center"> Wall-E Real bot </h3>
 </p>

How do you make a robot follow a line? Now what if the robot has only two wheels. How will it balance and follow the line? This is exactly what is taught in the Wall-E workshop.
The Wall-E workshop focuses on the concepts of line-following and self-balancing, using ESP32, a powerful micro-controller with features like inbuilt Wi-Fi support and BLE, with extensive application in several domains like IoT, RF, etc. The workshop entails various essential concepts such as PID Control, Embedded Communication Protocols, PWM, Filters, RTOS, etc. and gives its attendees (first-year students) a general idea about the world of robotics and embedded hardware. Following are some of the highlights:
- The custom-made SRA Development Board & ESP32.
- ESP-IDF, the official IoT Development Framework by Espressif.
- Motor Control with PWM and PID.
- Accelerometer , Gyroscope and Complimentary filter.
- Communcation protocols ( UART , SPI , I2C )


## Installations

- For Installations Please refer to [these instructions](Installations.md)
- In case of any errors , please refer to [Step-by-Step idf installation by espressif](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/#installation-step-by-step)

## Content

|                  TOPIC                  |                                                        Link                                                         |                                          Description                                           |
| :-------------------------------------: | :-----------------------------------------------------------------------------------------------------------------: | :--------------------------------------------------------------------------------------------: |
|                LED Blink                |              [LED_Blink](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/1_led_blink/README.md)               |                     Performing LED Blink by connecting the LED with esp32                      |
|           Light Sensing Array           |                    [LSA](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/2_LSA/README.md)                     |        Sensing Array that uses different Light Sensors, majorly used for Line-Following        |
|                 MPU6050                 |                    [MPU](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/3_MPU/README.md)                     |           Motion Controlled MPU measures angles with respect to 6 Degrees of Freedom           |
| Switch Controlled Motor in Normal Mode  |   [Normal_Mode](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/4_switch_controlled_motor_normal/readme.md)   |         Controls the movement of Bot manually using Switches connected in Normal Mode          |
| Switch Controlled Motor in Paralle Mode | [Parallel_Mode](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/5_switch_controlled_motor_parallel/README.md) |        Controls the movement of Bot manually using Switches connected in Parallel Mode         |
|         Pulse Width Modulation          |                    [PWM](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/6_PWM/README.md)                     |                    Calculate average Voltage and controls speed accordingly                    |
|             Line-Following              |              [Line_Following](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/tree/dev/7_line_following)               |         Line-Following Algorithm that enables the bot to follow the path of white line         |
|             Self-Balancing              |         [Self_Balancing](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/dev/8_self_balancing/README.md)          | Self-Balancing Algorithm that enables the bot to balance its weight under the force of gravity |

<!-- ROADMAP -->

## Roadmap

- See the [open issues](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/issues) for a list of proposed features (and known issues).
- Check out the [Custom-made SRA development board](https://github.com/SRA-VJTI/sra-board-hardware-design)
- Components of [SRA development board](https://sravjti.tech/sra-board-component/)
<p align="center">
  <img src="documentation/images/sra_board.png"/>
  <h3 align="center"> SRA development Board </h3>
  </p>

<!-- CONTRIBUTING -->

## Contributing

Contributions are what make the open source community such an amazing place to be learn, inspire, and create. Any contributions you make are **extremely appreciated**.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

<!-- LICENSE -->

## License

- Distributed under the [MIT License](https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/main/LICENSE).

<!-- CONTACT -->

## Contact

- [@SRAVJTI](https://twitter.com/SRAVJTI) - https://www.sravjti.in/
- [SRA-VJTI Website](https://www.sravjti.in/contact-us)


[forks-shield]:https://img.shields.io/github/forks/SRA-VJTI/Wall-E_v2.2-beta
[forks-url]: https://github.com/HarshShah03325/Wall-E_v2.2-beta/network/members
[stars-shield]: https://img.shields.io/github/stars/SRA-VJTI/Wall-E_v2.2-beta
[stars-url]: https://github.com/SRA-VJTI/Wall-E_v2.2-beta/stargazers
[issues-shield]: https://img.shields.io/github/issues/SRA-VJTI/Wall-E_v2.2-beta
[issues-url]: https://github.com/SRA-VJTI/Wall-E_v2.2-beta/issues
[license-shield]: https://img.shields.io/github/license/SRA-VJTI/Wall-E_v2.2-beta
[license-url]: https://github.com/SRA-VJTI/Wall-E_v2.2-beta/blob/master/LICENSE



