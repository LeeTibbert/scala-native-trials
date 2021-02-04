package java.lang

import java.io.InputStream

import scala.io.Source

object ProcessUtils {
  def readInputStream(s: InputStream) = Source.fromInputStream(s).mkString

  val resourceDir =
    s"${System.getProperty("user.dir")}/unit-tests/src/test/resources/process"

  val bashExtraPaths = List("/bin", "/usr/bin", "/usr/local/bin")
}
