[![✗](https://img.shields.io/badge/Release-v1.1.0-ffb600.svg?style=for-the-badge)](https://github.com/agustin-golmar/Flex-Bison-Compiler/releases)
[![✗](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml/badge.svg?branch=production)](https://github.com/agustin-golmar/Flex-Bison-Compiler/actions/workflows/pipeline.yaml)

# Flex-Bison-Compiler - Animato (G-73)

## Trabajo Práctico

Este proyecto fue desarrollado como trabajo práctico para la materia Autómatas,Teoría de Lenguajes y Compiladores. El compilador acepta código fuente escrito en un lenguaje estilo Angular y retorna como salida el código en CSS.

**Integrantes:**
- jobenegaslynch@itba.edu.ar - 64210
- ivilamowski@itba.edu.ar - 64242
- smaffeo@itba.edu.ar - 64245


## Tabla de Contenidos

* [Environment](#environment)
* [CI/CD](#cicd)
* [Recommended Extensions](#recommended-extensions)
* Installation
  * [Docker](doc/readme/Docker.md)
  * [Microsoft Windows](doc/readme/Windows.md)
  * [Ubuntu](doc/readme/Ubuntu.md)

## Environment

Set the following environment variables to control and configure the behaviour of the application:

| Name | Default | Description |
|------|:-------:|-------------|
| `LOG_IGNORED_LEXEMES` | `true` | When `true`, logs all of the ignored lexemes found with Flex at DEBUGGING level. To remove those logs from the console output set it to `false`. |
| `LOGGING_LEVEL` | `INFORMATION` | The minimum level to log in the console output. From lower to higher, the available levels are: `ALL`, `DEBUGGING`, `INFORMATION`, `WARNING`, `ERROR` and `CRITICAL`. |

## CI/CD

To trigger an automatic integration on every push or PR (_Pull Request_), you must activate _GitHub Actions_ in the _Settings_ tab. Use the following configuration:

| Key | Value |
|-----|-------|
| Actions permissions | Allow all actions and reusable workflows |
| Artifact and log retention | `30 days` |
| Fork pull request workflows from outside collaborators | Require approval for all outside collaborators |
| Workflow permissions | Read repository contents and packages permissions |
| Allow GitHub Actions to create and approve pull requests | `false` |

After integration is done, change the badges of this `README.md` to point to the new repository.

## Recommended Extensions

* [C/C++](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
* [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
* [Yash](https://marketplace.visualstudio.com/items?itemName=daohong-emilio.yash)  
