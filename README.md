# tcp-http-redis

# 这是一个http服务器
为用户提供查询节点列表服务；数据保存在redis数据库
# 这是一个redis数据库
保存一些共享数据，包括节点列表；数据库可以主从备份，可以集群
# 这是一个tcp服务器，也叫节点
每个节点支持10万用户连接，可以增加节点；
节点把自己的信息上报到redis数据库；
支持信息用密钥对加密，每个用户分配一个密钥对；必须拥有私钥才能查看信息才能查看
