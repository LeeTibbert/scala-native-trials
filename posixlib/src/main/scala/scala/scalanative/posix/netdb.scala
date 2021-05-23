package scala.scalanative.posix

import scalanative.unsafe._
import scalanative.posix.sys.socket
import scalanative.posix.netinet.in

@extern
object netdb {
  /* Offset of fields in 'addrinfo' differs between Linux, macOS,
   * and possibly others. Until a better way is developed, the '@name`
   * routines provide operating system independent handling (copy-in/copy-out)
   * of the structure. Study carefully the handling of ai_next in those
   * routines.
   */

  // This is the Linux layout. macOS pads between ai_addrlen & ai_addr.
  type addrinfo = CStruct8[CInt, // ai_flags
                           CInt,                 // ai_family
                           CInt,                 // ai_socktype
                           CInt,                 // ai_protocol
                           socket.socklen_t,     // ai_addrlen
                           Ptr[socket.sockaddr], // ai_addr
                           Ptr[CChar],           // ai_canonname
                           Ptr[Byte]]            // ai_next

  // functions

  @name("scalanative_freeaddrinfo")
  def freeaddrinfo(addr: Ptr[addrinfo]): Unit = extern

  @name("scalanative_getaddrinfo")
  def getaddrinfo(name: CString,
                  service: CString,
                  hints: Ptr[addrinfo],
                  res: Ptr[Ptr[addrinfo]]): CInt = extern

  def getnameinfo(addr: Ptr[socket.sockaddr],
                  addrlen: socket.socklen_t,
                  host: CString,
                  hostlen: socket.socklen_t,
                  service: CString,
                  servlen: socket.socklen_t,
                  flags: CInt): CInt = extern

  def gai_strerror(errcode: CInt): CString = extern

  // symbolic constants

  @name("scalanative_ai_numerichost")
  def AI_NUMERICHOST: CInt = extern

  @name("scalanative_ai_passive")
  def AI_PASSIVE: CInt = extern

  @name("scalanative_ai_numericserv")
  def AI_NUMERICSERV: CInt = extern

  @name("scalanative_ai_addrconfig")
  def AI_ADDRCONFIG: CInt = extern

  @name("scalanative_ai_v4mapped")
  def AI_V4MAPPED: CInt = extern

  @name("scalanative_ai_canonname")
  def AI_CANONNAME: CInt = extern
}

object netdbOps {
  import netdb._

  implicit class addrinfoOps(val ptr: Ptr[addrinfo]) extends AnyVal {
    def ai_flags: CInt                           = ptr._1
    def ai_family: CInt                          = ptr._2
    def ai_socktype: CInt                        = ptr._3
    def ai_protocol: CInt                        = ptr._4
    def ai_addrlen: socket.socklen_t             = ptr._5
    def ai_addr: Ptr[socket.sockaddr]            = ptr._6
    def ai_canonname: Ptr[CChar]                 = ptr._7
    def ai_next: Ptr[Byte]                       = ptr._8
    def ai_flags_=(v: CInt): Unit                = ptr._1 = v
    def ai_family_=(v: CInt): Unit               = ptr._2 = v
    def ai_socktype_=(v: CInt): Unit             = ptr._3 = v
    def ai_protocol_=(v: CInt): Unit             = ptr._4 = v
    def ai_addrlen_=(v: socket.socklen_t): Unit  = ptr._5 = v
    def ai_addr_=(v: Ptr[socket.sockaddr]): Unit = ptr._6 = v
    def ai_canonname_=(v: Ptr[CChar]): Unit      = ptr._7 = v
    def ai_next_=(v: Ptr[Byte]): Unit            = ptr._8 = v
  }
}
