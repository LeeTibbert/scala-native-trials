package native
package compiler
package pass

import scala.collection.mutable
import native.nir._
import native.util.ScopedVar, ScopedVar.scoped

/** Eliminates:
 *  - Op.Copy
 */
trait CopyLowering extends Pass {
  private val locals = new ScopedVar[mutable.Map[Local, Val]]

  private def collect(blocks: Seq[Block]): mutable.Map[Local, Val] = {
    val copies = mutable.Map.empty[Local, Val]

    blocks.foreach { b =>
      b.instrs.foreach {
        case Instr(Some(n), _, Op.Copy(v)) =>
          copies(n) = v
        case instr =>
          ()
      }
    }

    copies
  }

  override def onBlocks(blocks: Seq[Block]): Seq[Block] =
    scoped (
      locals := collect(blocks)
    ) {
      super.onBlocks(blocks)
    }

  override def onInstr(instr: Instr): Seq[Instr] = instr match {
    case Instr(_, _, _: Op.Copy) =>
      Seq()
    case _ =>
      super.onInstr(instr)
  }

  override def onVal(value: Val): Val = super.onVal(value match {
    case Val.Local(loc, _) if locals.contains(loc) =>
      locals(loc)
    case _ =>
      value
  })
}