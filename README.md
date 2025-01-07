![image5](https://github.com/user-attachments/assets/e060484a-5afe-4ef9-aa5b-9634f03bcfc0)

# Half-Life Elite

An original Half-Life mod from 2004 for pro half-life players. It was launched before Steam existed but did migrate to Steam when it first came online. It was one of only 16 games on Steam at the time so I presume we could re-upload it to Steam with the original app ID.

Steam APP id: (I think)
`1:418088`

Note: This code was last run on Visual Studio 2008

## Building locally

Compile and link both the hle.dll and client.dll and drop them into your locally installed app.


copy hle.dll and client.dll to your local installed destination:

```
C:\Program Files (x86)\Steam\steamapps\common\half-life\hle\dlls\hle.dll
C:\Program Files (x86)\Steam\steamapps\common\half-life\hle\cl_dlls\client.dll
```


## Setting up Steam app
Todo


## Creating a release

The release process should probably flow through steam and the `hle_install_dir` is the final installed files that need to be bundled into the app.
Make sure any new .dll files or asset changes make their way into that folder before bundling.
