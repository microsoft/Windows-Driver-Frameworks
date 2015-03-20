# Windows Driver Frameworks
The Windows Driver Frameworks (WDF) are a set of libraries that make
it simple to write high-quality device drivers.

## Goals for this project
Developers can use the MSDN reference documentation to learn about the
core concepts of WDF and the APIs available for use. Still, there's no
substitute for actual source code. That's why we've published
the source behind KMDF and UMDF v2 for anyone to dig through and debug
drivers with.

### Learning from the source

Unsure about what a particular WDF method is doing? Take a look at the
source. Our aim is to make the inner workings of WDF as transparent
for developers as possible.

*Note: As you experiment with WDF, you may come across undocumented
 behavior or APIS. We strongly advise against taking dependencies on
 that behavior as it's subject to change in future releases.*

### Debugging with the framework

Using the source in this repo, developers can perform step-through
debugging into the WDF source. This makes it much easier to follow
driver activity, understand interactions with the framework, and
diagnose issues.  Debugging can be done live by hooking onto a running
driver or after a crash by analyzing the dump file.  See the
[debugging
page](https://github.com/Microsoft/Windows-Driver-Frameworks/wiki/Debugging-with-WDF-Source
"Debugging with source") in the wiki for instructions.

## Scope

With this initial release, we've published the source behind KMDF and
UMDF v2. You'll find that a great deal of the source is shared
between the two. Driving the frameworks forward with a unified model
is a key priority for the WDF team.

## Contributing to WDF
See
[CONTRIBUTING.md](https://github.com/Microsoft/Windows-Driver-Frameworks/blob/master/CONTRIBUTING.md
"Contributing") for policies on pull-requests to this repo.

## FAQ about this repo
See the [FAQ
page](https://github.com/Microsoft/Windows-Driver-Frameworks/wiki/WDF-on-GitHub-FAQ
"WDF FAQ") in the Wiki.

## Licensing
WDF is licensed under the MIT License.

##Related Repos
Driver samples for Windows 10 now also live on GitHub at
[/Microsoft/Windows-Driver-Samples](https://github.com/Microsoft/windows-driver-samples
"Driver Samples")
