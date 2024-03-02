Public Class Form5
    Private Sub Label3_Click(sender As Object, e As EventArgs) Handles Label3.Click

    End Sub
    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button1.Click
        Dim oForm As New Form6()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
    Private Sub Button3_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button3.Click
        Dim oForm As New Form6()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
    Private Sub Button4_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button4.Click
        Dim oForm As New Form6()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles Button2.Click
        Dim oForm As New Form7()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub

    Private Sub Button5_Click(sender As Object, e As EventArgs) Handles Button5.Click
        Dim oForm As New Form9()

        Me.Hide()
        oForm.ShowDialog()

        Me.Close()
    End Sub
End Class