# REST Dummy DuT

This dummy provides the possibility to send and receive messages.

To build and execute you can use docker (optionally mount a config file).

The config file found in this folder is used as default.
Every line represents one supported operation, everything that is not found in this list is rejected by the DuT.

```shell
docker build -t your-image-name .
docker run -it --rm -p 9090:9090 -v $(pwd)/config:/opt/config:ro your-image-name
```

If you want to compile locally the following libraries are needed:

- libasio-dev
- libssl-dev

This implementation uses the [restbed](https://github.com/Corvusoft/restbed) framework as well as
the [OpenAPI Stub Generator](https://github.com/OpenAPITools/openapi-generator).

Restbed is provided as git submodule, use `git submodule update` or `git clone --recursive` in order to checkout with submodules, but only `--depth 1` is needed

The generated stub (from [OpenApi Spec](REST_Dummy_DuT.yaml)) has to be slightly modified,
see [generatedAPI/README.md](generatedAPI/README.md).
