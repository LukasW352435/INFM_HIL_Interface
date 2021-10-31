# REST Dummy DuT

This dummy provides the possibility to send and receive messages.

To build and execute you can use docker (optionally mount config file).

```shell
docker build -t your-image-name .
docker run -it --rm -p 9090:9090 -v $(pwd)/config:/opt/config your-image-name:ro
```

If you want to compile locally the following libraries are needed:

- libasio-dev
- libssl-dev

This implementation uses the [restbed](https://github.com/Corvusoft/restbed) framework as well as
the [OpenAPI Stub Generator](https://github.com/OpenAPITools/openapi-generator). Restbed is provided as git submodule,
use `git submodule update` or `git clone --recursive` to load it.

The generated stub (from [OpenApi Spec](REST_Dummy_DuT.yaml)) has to be slightly modified,
see [generatedAPI/README.md](generatedAPI/README.md).