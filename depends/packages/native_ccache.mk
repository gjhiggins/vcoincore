package=native_ccache
<<<<<<< HEAD
$(package)_version=3.2.4
$(package)_download_path=http://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=ffeb967edb549e67da0bd5f44f729a2022de9fdde65dfd80d2a7204d7f75332e
=======
$(package)_version=3.2.5
$(package)_download_path=http://samba.org/ftp/ccache
$(package)_file_name=ccache-$($(package)_version).tar.bz2
$(package)_sha256_hash=7a553809e90faf9de3a23ee9c5b5f786cfd4836bf502744bedb824a24bee1097
>>>>>>> official/0.13

define $(package)_set_vars
$(package)_config_opts=
endef

define $(package)_config_cmds
  $($(package)_autoconf)
endef

define $(package)_build_cmds
  $(MAKE)
endef

define $(package)_stage_cmds
  $(MAKE) DESTDIR=$($(package)_staging_dir) install
endef

define $(package)_postprocess_cmds
  rm -rf lib include
endef
